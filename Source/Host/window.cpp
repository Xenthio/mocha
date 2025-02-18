#include "window.h"

#include <SDL2/SDL_image.h>
#include <defs.h>
#include <gamesettings.h>
#include <globalvars.h>
#include <inputmanager.h>

#ifdef _IMGUI
#include <backends/imgui_impl_sdl.h>
#include <imgui.h>
#endif

#ifdef _WIN32
#include <SDL_syswm.h>
#include <Windows.h>
#endif

Window::Window( uint32_t width, uint32_t height )
{
	SDL_Init( SDL_INIT_VIDEO );

	SDL_WindowFlags windowFlags = ( SDL_WindowFlags )( SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN );
	m_visible = false;

	m_window = SDL_CreateWindow( WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags );

	// Check for any window creation errors and display them to the user. This can be things like, "we failed to create a
	// window because you don't have a Vulkan-capable GPU", so it's good to catch this early on.
	if ( m_window == nullptr )
	{
		const char* error = SDL_GetError();
		ErrorMessage( std::string( error ) );
		abort();
	}

	if ( !GameSettings::Get()->icon.empty() )
	{
		SDL_Surface* icon;
		icon = IMG_Load( GameSettings::Get()->icon.c_str() );
		SDL_SetWindowIcon( m_window, icon );
	}
	else
	{
		spdlog::warn( "No icon provided, so not using one. Make sure GameSettings has been filled correctly." );
	}
}

VkSurfaceKHR Window::CreateSurface( VkInstance instance )
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface( m_window, instance, &surface );
	return surface;
}

void Window::Cleanup()
{
	SDL_DestroyWindow( m_window );
}

void Window::Show()
{
	if ( m_visible )
		return;

	SDL_ShowWindow( m_window );
	m_visible = true;
}

bool Window::Update()
{
	SDL_Event e;
	bool bQuit = false;

	InputState inputState = g_inputManager->GetState();

	// Clear mouse delta every frame
	inputState.mouseDelta = { 0, 0 };

	SDL_SetRelativeMouseMode( m_captureMouse ? SDL_TRUE : SDL_FALSE );

	while ( SDL_PollEvent( &e ) != 0 )
	{
		if ( e.type == SDL_QUIT )
		{
			return true;
		}
		else if ( e.type == SDL_WINDOWEVENT )
		{
			SDL_WindowEvent we = e.window;

			if ( we.event == SDL_WINDOWEVENT_SHOWN )
			{
#if _WIN32
				// Force window dark mode
				auto dwm = LoadLibraryA( "dwmapi.dll" );
				auto uxtheme = LoadLibraryA( "uxtheme.dll" );

				typedef HRESULT ( *SetWindowThemePTR )( HWND, const wchar_t*, const wchar_t* );
				SetWindowThemePTR SetWindowTheme = ( SetWindowThemePTR )GetProcAddress( uxtheme, "SetWindowTheme" );

				typedef HRESULT ( *DwmSetWindowAttributePTR )( HWND, DWORD, LPCVOID, DWORD );
				DwmSetWindowAttributePTR DwmSetWindowAttribute =
				    ( DwmSetWindowAttributePTR )GetProcAddress( dwm, "DwmSetWindowAttribute" );

				SDL_SysWMinfo wmi;
				SDL_VERSION( &wmi.version );
				SDL_GetWindowWMInfo( SDL_GetWindowFromID( we.windowID ), &wmi );
				auto hwnd = wmi.info.win.window;

				SetWindowTheme( hwnd, L"DarkMode_Explorer", NULL );

				BOOL darkMode = 1;
				if ( FAILED( DwmSetWindowAttribute( hwnd, 20, &darkMode, sizeof( darkMode ) ) ) )
					DwmSetWindowAttribute( hwnd, 19, &darkMode, sizeof( darkMode ) );
#endif
			}
			else if ( we.event == SDL_WINDOWEVENT_SIZE_CHANGED )
			{
				if ( we.windowID == SDL_GetWindowID( m_window ) )
				{
					auto width = we.data1;
					auto height = we.data2;

					spdlog::info( "Window was resized to {}x{}", width, height );

					// Push event so that renderer etc. knows we've resized the window
					Size2D windowExtents = { ( uint32_t )width, ( uint32_t )height };

					if ( m_onWindowResized != nullptr )
						m_onWindowResized( windowExtents );
				}
			}
			else if ( we.event == SDL_WINDOWEVENT_CLOSE )
			{
				// We don't want to quit if an editor window is closed
				if ( we.windowID == SDL_GetWindowID( m_window ) )
				{
					// Main window was closed, so quit
					bQuit = true;
				}
			}
		}
		else if ( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP )
		{
			SDL_MouseButtonEvent mbe = e.button;

			bool isDown = mbe.state == SDL_PRESSED;

			if ( inputState.buttons.size() <= mbe.button )
				inputState.buttons.resize( mbe.button + 1 );

			inputState.buttons[mbe.button] = isDown;
		}
		else if ( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP )
		{
			SDL_KeyboardEvent kbe = e.key;

			bool isDown = kbe.state == SDL_PRESSED;
			int scanCode = kbe.keysym.scancode;

			if ( inputState.keys.size() <= scanCode )
				inputState.keys.resize( scanCode + 1 );

			inputState.keys[scanCode] = isDown;

			if ( kbe.keysym.scancode == SDL_SCANCODE_GRAVE && isDown )
				m_captureMouse = !m_captureMouse;
		}
		else if ( e.type == SDL_MOUSEMOTION )
		{
			SDL_MouseMotionEvent mme = e.motion;

			inputState.mousePosition = { ( float )mme.x, ( float )mme.y };

			if ( m_captureMouse )
			{
				inputState.mouseDelta.x += ( float )mme.xrel;
				inputState.mouseDelta.y += ( float )mme.yrel;
			}
		}

#ifdef _IMGUI
		// Pipe event to imgui too
		ImGui_ImplSDL2_ProcessEvent( &e );
#endif
	}

	g_inputManager->SetState( inputState );

	return bQuit;
}
