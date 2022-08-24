#include "CRenderer.h"

#include "Assert.h"
#include "CWindow.h"

#include <format>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdio.h>

CRenderer::CRenderer( CWindow* window )
{
	mWindow = window;

	InitAPI();
	InitSwapchain();

	InitCommands();

	InitDefaultRenderPass();
	InitFramebuffers();
}

CRenderer::~CRenderer()
{
	Cleanup();
}

void CRenderer::Resize( Uint2 size ) {}

void CRenderer::InitAPI()
{
	vkb::InstanceBuilder builder;
	auto inst_ret = builder.set_app_name( "Mocha Engine Game" )
	                    .request_validation_layers()
	                    .require_api_version( 1, 1, 0 )
	                    .use_default_debug_messenger()
	                    .build();

	if ( !inst_ret )
	{
		spdlog::error( "Failed to create Vulkan instance" );
	}

	vkb::Instance vkb_inst = inst_ret.value();

	mInstance = vkb_inst.instance;
	mDebugMessenger = vkb_inst.debug_messenger;

	//
	// Create a window surface using SDL
	//
	auto sdlWindow = mWindow->GetWindowPointer();
	if ( !SDL_Vulkan_CreateSurface( sdlWindow, mInstance, &mSurface ) )
	{
		spdlog::error( "Failed to create Vulkan surface" );
	}

	//
	// Select a GPU with min vk version 1.1 that can write to the SDL surface
	//
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector.set_minimum_version( 1, 1 ).set_surface( mSurface ).select().value();

	if ( !physicalDevice )
	{
		spdlog::error( "Failed to select a physical device" );
	}

	//
	// Create a logical device
	//
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();

	if ( !vkbDevice )
	{
		spdlog::error( "Failed to create logical device" );
	}

	mDevice = vkbDevice.device;
	mPhysicalDevice = physicalDevice.physical_device;

	//
	// Grab the queue and queue family
	//
	mGraphicsQueue = vkbDevice.get_queue( vkb::QueueType::graphics ).value();
	mGraphicsQueueFamily = vkbDevice.get_queue_index( vkb::QueueType::graphics ).value();
}

void CRenderer::InitSwapchain()
{
	//
	// Create a swapchain
	//
	vkb::SwapchainBuilder swapchainBuilder{ mPhysicalDevice, mDevice, mSurface };
	vkb::Swapchain swapchain = swapchainBuilder.use_default_format_selection()
	                               .set_desired_present_mode( VK_PRESENT_MODE_FIFO_KHR )
	                               .set_desired_extent( 1280, 720 )
	                               .build()
	                               .value();

	mSwapchain = swapchain.swapchain;
	mSwapchainImageFormat = swapchain.image_format;
	mSwapchainImages = swapchain.get_images().value();
	mSwapchainImageViews = swapchain.get_image_views().value();

	if ( mSwapchainImages.size() == 0 )
	{
		spdlog::error( "Failed to create swapchain" );
	}
}

void CRenderer::InitCommands()
{
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;

	commandPoolInfo.queueFamilyIndex = mGraphicsQueueFamily;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	ASSERT( vkCreateCommandPool( mDevice, &commandPoolInfo, nullptr, &mCommandPool ) );

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;

	commandBufferAllocateInfo.commandPool = mCommandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	ASSERT( vkAllocateCommandBuffers( mDevice, &commandBufferAllocateInfo, &mCommandBuffer ) );
}

void CRenderer::InitDefaultRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mSwapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	ASSERT( vkCreateRenderPass( mDevice, &renderPassInfo, nullptr, &mRenderPass ) );
}

void CRenderer::InitFramebuffers()
{
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = nullptr; // Does this seriously need to be set to nullptr here?
	
	framebufferInfo.renderPass = mRenderPass;
	framebufferInfo.width = 1280;
	framebufferInfo.height = 720;
	framebufferInfo.layers = 1;
	framebufferInfo.attachmentCount = 1;

	const size_t swapchainImageCount = mSwapchainImages.size();
	mFramebuffers = std::vector<VkFramebuffer>( swapchainImageCount );

	for ( size_t i = 0; i < swapchainImageCount; i++ )
	{
		framebufferInfo.pAttachments = &mSwapchainImageViews[i];
		ASSERT( vkCreateFramebuffer( mDevice, &framebufferInfo, nullptr, &mFramebuffers[i] ) );
	}
}

void CRenderer::Cleanup()
{
	vkDestroyCommandPool( mDevice, mCommandPool, nullptr );
	vkDestroySwapchainKHR( mDevice, mSwapchain, nullptr );

	vkDestroyRenderPass( mDevice, mRenderPass, nullptr );

	for ( size_t i = 0; i < mFramebuffers.size(); i++ )
	{
		vkDestroyFramebuffer( mDevice, mFramebuffers[i], nullptr );
		vkDestroyImageView( mDevice, mSwapchainImageViews[i], nullptr );
	}

	for ( size_t i = 0; i < mSwapchainImageViews.size(); i++ )
	{
		vkDestroyImageView( mDevice, mSwapchainImageViews[i], nullptr );
	}

	vkDestroyDevice( mDevice, nullptr );
	vkDestroySurfaceKHR( mInstance, mSurface, nullptr );

	vkb::destroy_debug_utils_messenger( mInstance, mDebugMessenger );

	vkDestroyInstance( mInstance, nullptr );
}

void CRenderer::BeginFrame() {}

void CRenderer::EndFrame() {}