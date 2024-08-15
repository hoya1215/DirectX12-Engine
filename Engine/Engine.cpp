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

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

//Engine::Engine()
//{
//	//m_engineInit = make_unique<EngineInit>();
//	m_timer->Init();
//
//}

void Engine::Init(const HWND& hwnd)
{
	m_hwnd = hwnd;

	// ��ġ �ʱ�ȭ
	CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	InitMainWindow();
	InitPrimitiveType();

	m_objHeap = make_shared<DescriptorHeap>();
	m_objHeap->Init();

	CreateFence();
	CheckMSAA();
	CreateCommand();
	CreateSwapChain();
	CreateDescriptorHeap();
	CreateRTV();
	CreateDSV();
	CreateViewPortandRect();


	m_timer->Init();
	m_pipeLine = make_shared<PipeLine>(m_device, m_commandList, m_resourceCmdList);
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

	ThrowIfFailed(m_commandAllocator->Reset());

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipeLine->GetPSO(PSO_TYPE::DEFAULT).Get()));

	//auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_PRESENT,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET);
	//m_commandList->ResourceBarrier(
	//	1, &toRenderTargetBarrier
	//);

	Util::ResourceStateTransition(m_swapChainBuffer[m_currentBackBuffer], D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_commandList->RSSetViewports(1, &m_viewPort);
	m_commandList->RSSetScissorRects(1, &m_rect);

	m_commandList->ClearRenderTargetView(
		GetCurrentBackBufferHandle(),
		Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(
		GetDepthStencilHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	m_deferred->ClearRenderTarget();


	m_dsvCPUHandle = GetDepthStencilHandle();

	//m_commandList->OMSetRenderTargets(1, &m_swapChainCPUHandle, true, &m_dsvCPUHandle);
}

void Engine::RenderEnd()
{
	//auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(
	//	GetCurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PRESENT);
	//m_commandList->ResourceBarrier(
	//	1, &toPresent
	//);
	Util::ResourceStateTransition(m_swapChainBuffer[m_currentBackBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	WaitSync();
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

	Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
		D3D12_RESOURCE_STATE_RENDER_TARGET, DEFERRED_COUNT);
	m_deferred->OMSetRenderTarget(m_dsvCPUHandle);
	// Root signature ����
	m_pipeLine->Render();

	

	// Light
	m_light->Render();

	// Global Data ����
	m_globalData->Render();

	

	m_filter->Render();

	// Deferred ����
	Deferred_Render();

	ShadowPass();



	m_swapChainCPUHandle = GetCurrentBackBufferHandle();
	

	
	m_postProcess->SetFiltersSRVHandle(m_filter->GetGPUSRVHandle());
	m_postProcess->CombineRender(m_deferred->m_deferredSRVHeapStartHandle);

	// compute shader
	// TODO

	//m_commandList->OMSetRenderTargets(1, &m_swapChainCPUHandle, true, &m_dsvCPUHandle);
	m_commandList->OMSetRenderTargets(1, &m_swapChainCPUHandle, false, nullptr);
	m_postProcess->PostRender();

	if(b_useImGui)
		ImGuiRender();

	RenderEnd();


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

void Engine::CreateCommand()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	
	// Queue
	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(m_commandQueue.GetAddressOf())));


	// Allocator
	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	IID_PPV_ARGS(m_resourceAlloc.GetAddressOf())));


	// CmdList
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf())));
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_resourceAlloc.Get(), nullptr, IID_PPV_ARGS(m_resourceCmdList.GetAddressOf())));

	m_commandList->Close();

	m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
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
	HRESULT hr = m_factory->CreateSwapChain(
		m_commandQueue.Get(),
		&desc,
		&m_swapChain
	);

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
	m_commandList->ResourceBarrier(
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

	m_commandList->RSSetViewports(1, &m_viewPort);
	m_commandList->RSSetScissorRects(1, &m_rect);


}

void Engine::WaitSync()
{
	m_fenceValue++;

	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void Engine::CloseResourceCmdList()
{
	m_resourceCmdList->Close();

	ID3D12CommandList* cmdList[] = { m_resourceCmdList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

	WaitSync();

	m_resourceAlloc->Reset();
	m_resourceCmdList->Reset(m_resourceAlloc.Get(), nullptr);
}



void Engine::ShowFPS()
{
	uint32 fps = m_timer->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(m_hwnd, text);
}

void Engine::ShadowPass()
{
	m_commandList->ClearDepthStencilView(
		m_shadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	CMD_LIST->OMSetRenderTargets(0, nullptr, FALSE, &m_shadowMapCPUHandle);

	m_objectManager->ShadowRender();

	Util::ResourceStateTransition(m_shadowMapBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Engine::Deferred_Render()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get() };
	//ID3D12DescriptorHeap* descriptorHeaps[] = { g_engine->m_deferred->m_deferredSRVHeap.Get()};
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_deferred->OMSetRenderTarget(m_dsvCPUHandle);

	// deferred ��ü����
	// Object ����
	m_objectManager->Render();

	Util::ResourceStateTransition(m_deferred->m_deferredRTVBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, DEFERRED_COUNT);

}

void Engine::ImGuiRender()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetImGuiSRVHeap().Get()};
	//ID3D12DescriptorHeap* descriptorHeaps[] = { g_engine->m_deferred->m_deferredSRVHeap.Get()};
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CMD_LIST.Get());
}
