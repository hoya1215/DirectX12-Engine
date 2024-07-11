#pragma once


//#include "pch.h"
//#include "Headers.h"
#include "Timer.h"

class KeyInput;

class Engine
{
public:
	//Engine();


	void Init(const HWND& hwnd);
	void Update();
	void RenderBegin();
	void Render();
	void RenderEnd();
	void ShowFPS();

	// 장치 초기화
	void InitMainWindow();
	void CreateFence();
	void CheckMSAA();
	void CreateCommand();
	void CreateSwapChain();
	void CreateDescriptorHeap();
	void CreateRTV();
	void CreateDSV();
	void CreateViewPortandRect();

	void WaitSync();
	void CloseResourceCmdList();



	ID3D12Resource* GetCurrentBackBuffer() { return m_swapChainBuffer[m_currentBackBuffer].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle() {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackBuffer,
			m_rtvHeapSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilHandle() { return m_dsvHeap->GetCPUDescriptorHandleForHeapStart(); }

	// 장치 가져오기
	ComPtr<ID3D12Device>& GetDevice() { return m_device; }
	ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return m_commandList; }
	ComPtr<ID3D12GraphicsCommandList>& GetResCmdList() { return m_resourceCmdList; }
	shared_ptr<KeyInput> GetKeyInput() { return m_keyInput; }

	float m_deltaTime = 0;

private:
	int m_width = 1280;
	int m_height = 720;
	//unique_ptr<class EngineInit> m_engineInit;
	unique_ptr<Timer> m_timer = make_unique<Timer>();
	

	static const int SwapChainBufferCount = 2;

	HINSTANCE m_hInst;
	HWND m_hwnd;
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIFactory> m_factory;
	ComPtr<ID3D12PipelineState> m_pipelineState;

	// SwapChain
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount];
	int m_currentBackBuffer = 0;

	// Command
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	ComPtr<ID3D12CommandAllocator> m_resourceAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_resourceCmdList;

	// DepthStencil
	ComPtr<ID3D12Resource> m_depthStencilBuffer;

	// Descriptor Heap
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHeapHandle[SwapChainBufferCount];
	uint32 m_rtvHeapSize = 0;

	// PipeLine
	shared_ptr<class PipeLine> m_pipeLine;


	// fence
	ComPtr<ID3D12Fence> m_fence;
	uint32 m_fenceValue =0;
	HANDLE	m_fenceEvent = INVALID_HANDLE_VALUE;

	// ViewPort , ScissorRect
	D3D12_VIEWPORT m_viewPort;
	RECT m_rect;

	// MSAA
	bool b_msaa = false;
	uint32 m_msaaQuality;

	// Input
	shared_ptr<KeyInput> m_keyInput;

};

