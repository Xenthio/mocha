#pragma once
#include <vk_mem_alloc.h>

// TODO: Remove
enum RenderDebugViews
{
	NONE = 0,
	DIFFUSE = 1,
	NORMAL = 2,
	AMBIENTOCCLUSION = 3,
	METALNESS = 4,
	ROUGHNESS = 5,

	OTHER = 63
};

class RenderManager;
class RenderdocManager;
class HostManager;
class LogManager;
class EntityManager;
class PhysicsManager;
class InputManager;
class BaseRenderContext;
class GameSettings;
class CVarManager;

struct Vector3;
struct Quaternion;

//
// Global vars
//
extern GameSettings* g_gameSettings;
extern RenderManager* g_renderManager;
extern LogManager* g_logManager;
extern HostManager* g_hostManager;
extern RenderdocManager* g_renderdocManager;
extern EntityManager* g_entityDictionary;
extern PhysicsManager* g_physicsManager;
extern InputManager* g_inputManager;
extern BaseRenderContext* g_renderContext;
extern CVarManager* g_cvarManager;

extern float g_curTime;
extern float g_frameTime;
extern float g_tickTime;
extern int g_curTick;

extern Vector3 g_cameraPos;
extern Quaternion g_cameraRot;
extern float g_cameraFov;
extern float g_cameraZNear;
extern float g_cameraZFar;

extern RenderDebugViews g_debugView;