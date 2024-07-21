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

//Engine::Engine()
//{
//	//m_engineInit = make_unique<EngineInit>();
//	m_timer->Init();
//
//}

void Engine::Init(const HWND& hwnd)
{
	m_hwnd = hwnd;

	// 장치 초기화
	CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	InitMainWindow();
	CreateFence();
	CheckMSAA();
	CreateCommand();
	CreateSwapChain();
	CreateDescriptorHeap();
	CreateRTV();
	CreateDSV();
	CreateViewPortandRect();
	m_objHeap = make_shared<DescriptorHeap>();
	m_objHeap->Init();

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
	m_mainCamera->AddComponent(COMPONENT_TYPE::BEHAVIOUR, CM);
}

void Engine::Update()
{
	m_deltaTime = m_timer->GetDeltaTime();
	m_keyInput->Update();

	//m_pipeLine->Update();
	m_mainCamera->Update();
	m_globalData->Update();

	m_objectManager->Update();

	m_timer->Update();
	ShowFPS();
}

void Engine::RenderBegin()
{

	ThrowIfFailed(m_commandAllocator->Reset());

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipeLine->GetPSO(PSO_TYPE::DEFAULT).Get()));

	auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(
		1, &toRenderTargetBarrier
	);

	m_commandList->RSSetViewports(1, &m_viewPort);
	m_commandList->RSSetScissorRects(1, &m_rect);

	m_commandList->ClearRenderTargetView(
		GetCurrentBackBufferHandle(),
		Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(
		GetDepthStencilHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentBackBufferHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDepthStencilHandle();
	m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
}

void Engine::RenderEnd()
{
	auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(
		1, &toPresent
	);

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	WaitSync();
}

void Engine::Render()
{
	RenderBegin();

	Update();

	// Root signature 세팅
	m_pipeLine->Render();

	// Global Data 세팅
	m_globalData->Render();

	// Object 렌더
	m_objectManager->Render();

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
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	dsvDesc.NumDescriptors = 1;
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

	m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_dsvHeapStart);


	auto toDepthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandList->ResourceBarrier(
		1, &toDepthBarrier

	);

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