#include "pch.h"
#include "Engine.h"
#include "EngineInit.h"
#include "PipeLine.h"
#include "KeyInput.h"
#include "DescriptorHeap.h"
#include "ObjectManager.h"
#include "GlobalData.h"
#include "CameraMove.h"
#include "Camera.h"
#include "Light.h"
#include "RenderTargets.h"
#include "PostProcess.h"
#include "Util.h"
#include "Frustum.h"
#include "Filter.h"
#include "CommandManager.h"
#include "ThreadManager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

// 패스 단위 
#define MULTI_THREADING 1
#define THREAD_COUNT 2

void Engine::Init(const HWND& hwnd)
{
	m_hwnd = hwnd;

	// 장치 초기화
	CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	InitMainWindow();
	InitPrimitiveType();

	m_objHeap = make_shared<DescriptorHeap>();
	m_objHeap->Init();

	CreateFence();
	CheckMSAA();

	m_commandManager = make_shared<CommandManager>();
	m_commandManager->Create();

	m_threadManager = make_shared<ThreadManager>(THREAD_COUNT);
	m_threadManager->Init();

	CreateSwapChain();
	CreateDescriptorHeap();
	CreateRTV();
	CreateDSV();
	CreateViewPortandRect();


	m_timer->Init();
	m_pipeLine = make_shared<PipeLine>(m_device);
	m_pipeLine->Init();

	m_keyInput = make_shared<KeyInput>();
	m_keyInput->Init();

	m_objectManager = make_shared<ObjectManager>();
	m_objectManager->Init();

	m_globalData = make_shared<GlobalData>();
	m_globalData->Init();

	m_mainCamera = make_shared<Camera>("MainCamera");
	shared_ptr<CameraMove> CM = make_shared<CameraMove>();
	m_mainCamera->AddComponent(COMPONENT_TYPE::BEHAVIOR, CM);

	m_light = make_shared<Light>();
	m_light->AddLight(LIGHT_TYPE::DIRECTIONAL, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.f, 10.f, 5.f, 1.f), Vector4(0.f, -1.f, 0.f, 0.f));

	m_deferred = make_shared<RenderTargets>();
	m_deferred->Create();

	m_postProcess = make_shared<PostProcess>();

	m_filter = make_shared<Filter>();
	m_filter->Init(COMPUTE_PSO_TYPE::FILTER);

	m_frustum = make_shared<Frustum>();

	if (b_useImGui)
		InitImGui();
}

void Engine::Update()
{
	m_deltaTime = m_timer->GetDeltaTime();
	m_keyInput->Update();

	//m_pipeLine->Update();
	m_mainCamera->Update();
	m_globalData->Update();
	m_light->Update();
	m_objectManager->Update();

	m_timer->Update();
	ShowFPS();
}

void Engine::UpdateImGui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void Engine::RenderBegin()
{

#if MULTI_THREADING
	m_threadManager->ClearCommandList(m_viewPort, m_rect);
#else
	m_commandManager->Reset();
	m_commandManager->SetViewPort(m_viewPort, m_rect);
	m_commandManager->Clear();
#endif
}

void Engine::RenderEnd()
{

#if MULTI_THREADING

	m_threadManager->ClearEvent();

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	WaitSync(m_threadManager->GetCmdQueue());
#else
	m_commandManager->Close();
	
	m_commandManager->ExecuteCommandLists();

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	WaitSync(m_commandManager->GetCmdQueue());
#endif

}

void Engine::Render()
{
	if (b_useImGui)
	{
		UpdateImGui();
		ImGui::Render();
	}

	RenderBegin();

	Update();

	m_swapChainCPUHandle = GetCurrentBackBufferHandle();


#if MULTI_THREADING
	// Thread
	for (int i = 0; i < m_threadManager->GetThreads().size(); ++i)
	{
		Setting(m_threadManager->GetThreads()[i].m_commandList);
	}
	Setting(m_threadManager->m_finalCommandList);


	//m_threadManager->AddEvent(&Engine::BeginFrame, this);
	m_threadManager->AddEvent(&Engine::MiddleFrame1, this);
	m_threadManager->AddEvent(&Engine::MiddleFrame2, this);
	//m_threadManager->AddEvent(&Engine::MiddleFrame3, this);
	//m_threadManager->AddEvent(&Engine::MiddleFrame4, this);

	BeginThread();

	// Combine , PostProcess
	EndFrame(m_threadManager->m_finalCommandList, m_threadManager->m_commandQueue);
#else
	Setting(m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN));
	Setting(m_commandManager->GetCmdList(COMMANDLIST_TYPE::SHADOW));

	ShadowPass(m_commandManager->GetCmdList(COMMANDLIST_TYPE::SHADOW));
	Deferred_Render(m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN));

	PostRender(m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN));


#endif


	RenderEnd();


}

void Engine::BeginFrame(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	cmdList->ClearDepthStencilView(
		m_shadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, cmdList);

	m_deferred->ClearRenderTarget(cmdList);

	Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET, DEFERRED_COUNT, cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	WaitSync(cmdQueue);

	//m_filter->Render(cmdList);


}

void Engine::MiddleFrame1(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{

	//
	//cmdList->ClearDepthStencilView(
	//	m_shadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	//	1.0f, 0, 0, nullptr);
	//
	//Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE, cmdList);
	//cmdList->OMSetRenderTargets(0, nullptr, FALSE, &m_shadowMapCPUHandle);
	//
	//m_objectManager->ShadowRender1(cmdList);


	// 2개
	ShadowPass(cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);



	//WaitSync(cmdQueue);

}

void Engine::MiddleFrame2(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	//cmdList->ClearDepthStencilView(
	//	m_shadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	//	1.0f, 0, 0, nullptr);
	//
	//Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE, cmdList);
	//cmdList->OMSetRenderTargets(0, nullptr, FALSE, &m_shadowMapCPUHandle);
	//
	//m_objectManager->ShadowRender2(cmdList);
	//
	//Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, cmdList);

	Deferred_Render(cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);



	//WaitSync(cmdQueue);
}

void Engine::MiddleFrame3(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	//m_deferred->ClearRenderTarget(cmdList);
	//
	//Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, DEFERRED_COUNT, cmdList);

	m_deferred->OMSetRenderTarget(cmdList, m_dsvCPUHandle);

	m_objectManager->Render1(cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);



}

void Engine::MiddleFrame4(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	m_deferred->OMSetRenderTarget(cmdList, m_dsvCPUHandle);

	m_objectManager->Render2(cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);



	//Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, DEFERRED_COUNT, cmdList);
}

void Engine::EndFrame(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue>& cmdQueue)
{

	PostRender(cmdList);

	cmdList->Close();
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

void Engine::Setting(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	// Root signature 세팅
	m_pipeLine->Render(cmdList);

	// Light
	m_light->Render(cmdList);

	// Global Data 세팅
	m_globalData->Render(cmdList);

	SetDescriptorHeaps(cmdList);
}

void Engine::SetDescriptorHeaps(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get() };

	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void Engine::BeginThread()
{
	m_threadManager->BeginEvent();

}

void Engine::ShadowPass(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	cmdList->ClearDepthStencilView(
		m_shadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, cmdList);
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &m_shadowMapCPUHandle);

	m_objectManager->ShadowRender(cmdList);

	Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, cmdList);



}

void Engine::Deferred_Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

	m_deferred->ClearRenderTarget(cmdList);

	Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET, DEFERRED_COUNT, cmdList);

	m_deferred->OMSetRenderTarget(cmdList, m_dsvCPUHandle);

	// deferred 물체렌더
	// Object 렌더
	m_objectManager->Render(cmdList);

	Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, DEFERRED_COUNT, cmdList);

}


void Engine::PostRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Util::ResourceStateTransition(m_swapChainBuffer[m_currentBackBuffer], D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET, cmdList);


	m_postProcess->SetFiltersSRVHandle(m_filter->GetGPUSRVHandle());
	m_postProcess->CombineRender(cmdList, m_deferred->m_deferredSRVHeapStartHandle);

	// compute shader
	// TODO


	cmdList->OMSetRenderTargets(1, &m_swapChainCPUHandle, false, nullptr);

	m_postProcess->PostRender(cmdList);

	if (b_useImGui)
		ImGuiRender(cmdList);

	Util::ResourceStateTransition(m_swapChainBuffer[m_currentBackBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT, cmdList);
}

void Engine::WaitSync(ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	m_fenceValue++;

#if MULTI_THREADING
	cmdQueue->Signal(m_fence.Get(), m_fenceValue);
#else
	m_commandManager->GetCmdQueue()->Signal(m_fence.Get(), m_fenceValue);
#endif



	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void Engine::CloseResourceCmdList()
{
	m_commandManager->GetResourceCmdList()->Close();

	ID3D12CommandList* cmdList[] = { m_commandManager->GetResourceCmdList().Get() };

#if MULTI_THREADING
	m_threadManager->GetCmdQueue()->ExecuteCommandLists(_countof(cmdList), cmdList);
	WaitSync(m_threadManager->m_commandQueue);
#else
	m_commandManager->GetCmdQueue()->ExecuteCommandLists(_countof(cmdList), cmdList);
	WaitSync(m_commandManager->GetCmdQueue());
#endif


	WaitSync(m_threadManager->m_commandQueue);

	m_commandManager->GetResourceCmdAlloc()->Reset();
	m_commandManager->GetResourceCmdList()->Reset(m_commandManager->GetResourceCmdAlloc().Get(), nullptr);

}

void Engine::InitMainWindow()
{

	int count = 0;
	RECT R = { 0, 0, static_cast<long>(m_width), static_cast<long>(m_height) };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);


	if (!SetWindowPos(m_hwnd, 0, 100, 100, m_width, m_height, 0))
		count = 2;

}

void Engine::InitPrimitiveType()
{
	m_primitiveTypes.insert({ PRIMITIVE_TYPE::POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST });
	m_primitiveTypes.insert({ PRIMITIVE_TYPE::LINE, D3D_PRIMITIVE_TOPOLOGY_LINELIST });
	m_primitiveTypes.insert({ PRIMITIVE_TYPE::TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST });
	m_primitiveTypes.insert({ PRIMITIVE_TYPE::PATCH, D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST });
}

void Engine::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	RECT rect;
	GetClientRect(m_hwnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
	//io.DisplaySize = ImVec2(1280.f, 720.f);

	float dpiScale = GetDpiForWindow(m_hwnd) / 96.0f;
	ImGui::GetIO().DisplayFramebufferScale = ImVec2(dpiScale, dpiScale);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX12_Init(DEVICE.Get(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, OBJ_HEAP->GetImGuiSRVHeap().Get(),
		OBJ_HEAP->GetImGuiSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		OBJ_HEAP->GetImGuiSRVHeap()->GetGPUDescriptorHandleForHeapStart());
}

void Engine::CreateFence()
{
	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	//m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Engine::CheckMSAA()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms;
	ms.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ms.SampleCount = 4;
	ms.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	ms.NumQualityLevels = 0;

	ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&ms,
		sizeof(ms)));

	m_msaaQuality = ms.NumQualityLevels;
	assert(m_msaaQuality > 0 && "Unexpected MSAA quality level");
}


void Engine::CreateSwapChain()
{
	m_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferDesc.Width = m_width;
	desc.BufferDesc.Height = m_height;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	//desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = SwapChainBufferCount;
	desc.OutputWindow = m_hwnd;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//m_factory->CreateSwapChain(m_commandQueue.Get(), &desc,
	//	&m_swapChain);
	int count = 0;

#if MULTI_THREADING
	HRESULT hr = m_factory->CreateSwapChain(
		m_threadManager->GetCmdQueue().Get(),
		&desc,
		&m_swapChain
	);
#else
	HRESULT hr = m_factory->CreateSwapChain(
		m_commandManager->GetCmdQueue().Get(),
		&desc,
		&m_swapChain
	);
#endif


	if (FAILED(hr))
	{
		count = 2;
	}
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));
	}


}



void Engine::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.NumDescriptors = SwapChainBufferCount;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvDesc,
		IID_PPV_ARGS(m_rtvHeap.GetAddressOf()))); 

	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc;
	dsvDesc.NumDescriptors = DepthStencilBufferCount;
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDesc.NodeMask = 0;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvDesc,
		IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));

	m_rtvHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Engine::CreateRTV()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHeapStart(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		//m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));

		m_rtvHeapHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapStart, i * m_rtvHeapSize);
		m_device->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, m_rtvHeapHandle[i]);

		//m_rtvHeapStart.Offset(1, m_rtvHeapSize);
	}

}

void Engine::CreateDSV()
{
	m_dsvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_width;
	depthStencilDesc.Height = m_height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = b_msaa ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = b_msaa ? (m_msaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;


	auto heaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		m_device->CreateCommittedResource((&heaptype),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())));

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_dsvHeapStart(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	m_dsvIndex++;
	m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_dsvHeapStart);


	auto toDepthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN)->ResourceBarrier(
		1, &toDepthBarrier

	);

	// Shadow Map
	D3D12_RESOURCE_DESC shadowDSVDesc;
	shadowDSVDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	shadowDSVDesc.Alignment = 0;
	shadowDSVDesc.Width = m_width;
	shadowDSVDesc.Height = m_height;
	shadowDSVDesc.DepthOrArraySize = 1;
	shadowDSVDesc.MipLevels = 1;
	shadowDSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//shadowDSVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDSVDesc.SampleDesc.Count = b_msaa ? 4 : 1;
	shadowDSVDesc.SampleDesc.Quality = b_msaa ? (m_msaaQuality - 1) : 0;
	shadowDSVDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	shadowDSVDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE shadowOptClear;
	shadowOptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//shadowOptClear.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowOptClear.DepthStencil.Depth = 1.0f;
	shadowOptClear.DepthStencil.Stencil = 0;
	
	
	//auto heaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		m_device->CreateCommittedResource((&heaptype),
			D3D12_HEAP_FLAG_NONE,
			&shadowDSVDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&shadowOptClear,
			IID_PPV_ARGS(m_shadowMapBuffer.GetAddressOf())));
	
	m_shadowMapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), m_dsvIndex, m_dsvHeapSize);
	m_shadowMapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_dsvHeap->GetGPUDescriptorHandleForHeapStart(), m_dsvIndex, m_dsvHeapSize);
	m_dsvIndex++; 
	// Index assert

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	
	m_device->CreateDepthStencilView(m_shadowMapBuffer.Get(), &dsvDesc, m_shadowMapCPUHandle);
	
	
	uint32 srvIndex = OBJ_HEAP->GetPostSRVIndex();
	m_shadowMapSRVCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetPostSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvIndex, OBJ_HEAP->GetHeapSize());
	m_shadowMapSRVGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetPostSRVHeap()->GetGPUDescriptorHandleForHeapStart(),
		srvIndex, OBJ_HEAP->GetHeapSize());
	Util::CreateSRV(m_shadowMapBuffer, m_shadowMapSRVCPUHandle, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	//Util::CreateSRV(m_shadowMapBuffer, m_shadowMapSRVCPUHandle, DXGI_FORMAT_R32_TYPELESS);


	m_dsvCPUHandle = GetDepthStencilHandle();
}

void Engine::CreateViewPortandRect()
{
	
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = static_cast<float>(m_width);
	m_viewPort.Height = static_cast<float>(m_height);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_rect = { 0, 0, static_cast<long>(m_width), static_cast<long>(m_height) };



}





void Engine::ShowFPS()
{
	uint32 fps = m_timer->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(m_hwnd, text);
}





void Engine::ImGuiRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetImGuiSRVHeap().Get()};
	//ID3D12DescriptorHeap* descriptorHeaps[] = { g_engine->m_deferred->m_deferredSRVHeap.Get()};
	//m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandManager->GetCmdList(COMMANDLIST_TYPE::MAIN).Get());
}

