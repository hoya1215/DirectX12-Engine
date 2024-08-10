#pragma once

#define _HAS_STD_BYTE 0

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

#include "d3dx12.h"
#include <d3d12.h>
#include "d3dUtil.h"
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include "EngineInit.h"
#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <directXTex/DirectXTex.inl>

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "DirectXTK12")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

// 각종 typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;

#define DEVICE g_engine->GetDevice()
#define CMD_LIST g_engine->GetCmdList()
#define RES_CMD_LIST g_engine->GetResCmdList()
#define INPUT g_engine->GetKeyInput()
#define DT g_engine->m_deltaTime
#define VI_pair pair<vector<Vertex>, vector<uint16>>
#define OBJ_HEAP g_engine->GetHeap()
#define PIPELINE g_engine->GetPipeLine()
#define MAIN_CAMERA g_engine->GetMainCamera()


enum class PSO_TYPE
{
	DEFAULT,
	DEFERRED,
	DEFERRED_WIRE,
	DEFERRED_TS,
	INSTANCING,
	POST_PROCESS,
	SKYBOX
};

enum class COMPUTE_PSO_TYPE
{
	FILTER
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	SPOT
};


//Matrix CreateMatrix(Vector3 position, Vector3 scale = Vector3(1.f, 1.f, 1.f), Vector3 rotation = Vector3(0.f, 0.f, 0.f))
//{
//    return Matrix::CreateScale(scale) * Matrix::CreateRotationX(rotation.x)
//        * Matrix::CreateRotationY(rotation.y) * Matrix::CreateRotationZ(rotation.z)
//        * Matrix::CreateTranslation(position);
//}

class Engine;

extern unique_ptr<Engine> g_engine; // Engine 안에 있는 device 등을 가져오기 위해서 



