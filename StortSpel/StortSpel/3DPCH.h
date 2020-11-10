#pragma once

#ifndef UNICODE
#define UNICODE
#endif

// Standard
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <wrl/client.h>
#define NOMINMAX
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <ctime>
#include <memory>
#include <math.h>
#include <fstream>
#include <stack>
#include <iostream>
#include <chrono>
#include <cmath>

// DirectX 11
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include <DirectXCollision.h>
#include <Effects.h>
#include <DirectXColors.h>
#include <Audio.h>
#include <Dbt.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

//PhysX
#include"PxPhysicsAPI.h"

// ImGui includes
#include "../3rd-Party/ImGui/imconfig.h"
#include "../3rd-Party/ImGui/imgui.h"
#include "../3rd-Party/ImGui/imgui_impl_dx11.h"
#include "../3rd-Party/ImGui/imgui_impl_win32.h"
#include "../3rd-Party/ImGui/imgui_internal.h"
#include "../3rd-Party/ImGui/imstb_rectpack.h"
#include "../3rd-Party/ImGui/imstb_textedit.h"
#include "../3rd-Party/ImGui/imstb_truetype.h"

// Singletons
//#include "ResourceHandler.h"
#include "ErrorLogger.h"
#include "Buffer.h"
//#include "Engine.h"
//#include "Renderer.h"

// Misc
//#include "Transform.h"

// Components
//#include "Component.h"
//#include "TestComponent.h"
//#include "InvalidComponent.h"
//#include "MeshComponent.h"

//Texture loading
#include <WICTextureLoader.h>
#include<DDSTextureLoader.h>
using namespace DirectX;
using namespace SimpleMath;

//Include light structures
#include "LightStructs.h"

// DirectX 11 Linking
#pragma comment (lib, "user32")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "gdi32")
#pragma comment (lib, "d3dcompiler.lib")

#include "PerformanceTester.h"

//Defines
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;


enum class TriggerType
{
	UNDEFINED,
	EVENT,
	PICKUP,
	CHECKPOINT,
	PROJECTILE,
};

enum class EventType
{
	SWAPSCENE,
	BOSS_PROJECTILE_HIT,
};