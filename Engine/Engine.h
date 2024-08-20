#pragma once


//#include "pch.h"
//#include "Headers.h"
#include "Timer.h"


class KeyInput;
class PipeLine;
class DescriptorHeap;
class ObjectManager;
class GlobalData;
class Camera;
class Light;
class RenderTargets;
class PostProcess;
class Frustum;
class Filter;
class CommandManager;

class Engine
{
public:
	//Engine();


	void Init(const HWND& hwnd);
	void Update();
	void UpdateImGui();
	void RenderBegin();
	void Render();
	void RenderEnd();
	void ShowFPS();

	void ShadowPass();
	void Deferred_Render();
	void ImGuiRender();

	// ��ġ �ʱ�ȭ
	void InitMainWindow();
	void InitPrimitiveType();
	void InitImGui();
	void CreateFence();
	void CheckMSAA();
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

	// ��ġ ��������
	HWND GetHWND() { return m_hwnd; }
	ComPtr<ID3D12Device>& GetDevice() { return m_device; }
	shared_ptr<PipeLine> GetPipeLine() { return m_pipeLine; }
	shared_ptr<KeyInput> GetKeyInput() { return m_keyInput; }
	shared_ptr<DescriptorHeap> GetHeap() { return m_objHeap; }
	shared_ptr<Camera> GetMainCamera() { return m_mainCamera; }
	shared_ptr<Frustum> GetFrustum() { return m_frustum; }
	D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType(PRIMITIVE_TYPE primitiveType) { return m_primitiveTypes[primitiveType]; }
	shared_ptr<RenderTargets> GetRenderTargets() { return m_deferred; }
	shared_ptr<CommandManager> GetCommandManager() { return m_commandManager; }

	float m_deltaTime = 0;

	int m_width = 1280;
	int m_height = 720;

	// ImGui
	bool b_useImGui = false;
	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	Vector4 clear_color = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// Deferred
	shared_ptr<RenderTargets> m_deferred;

private:

	//unique_ptr<class EngineInit> m_engineInit;
	unique_ptr<Timer> m_timer = make_unique<Timer>();
	

	static const int SwapChainBufferCount = 2;
	static const int DepthStencilBufferCount = 2;

	HINSTANCE m_hInst;
	HWND m_hwnd;
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIFactory> m_factory;
	ComPtr<ID3D12PipelineState> m_pipelineState;

	// SwapChain
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount];
	int m_currentBackBuffer = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE m_swapChainCPUHandle;

	// Command
	shared_ptr<CommandManager> m_commandManager;

	ComPtr<ID3D12RootSignature> m_rootSignature;



	// DepthStencil
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvCPUHandle;

	ComPtr<ID3D12Resource> m_shadowMapBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_shadowMapCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_shadowMapGPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_shadowMapSRVCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_shadowMapSRVGPUHandle;

	// Descriptor Heap
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	uint32 m_dsvIndex = 0;
	uint32 m_dsvHeapSize = 0;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHeapHandle[SwapChainBufferCount];
	uint32 m_rtvHeapSize = 0;
	shared_ptr<DescriptorHeap> m_objHeap;

	// PipeLine
	shared_ptr<class PipeLine> m_pipeLine;

	// Primitive Type
	unordered_map< PRIMITIVE_TYPE, D3D_PRIMITIVE_TOPOLOGY> m_primitiveTypes;

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

	// Camera
	shared_ptr<Camera> m_mainCamera;

	// ObjectManager
	shared_ptr<ObjectManager> m_objectManager;

	// Light
	shared_ptr<Light> m_light;

	// Global Data
	shared_ptr<GlobalData> m_globalData;



	// PostProcess
	shared_ptr<PostProcess> m_postProcess;

	// Filter
	shared_ptr<Filter> m_filter;

	// Frustum
	shared_ptr<Frustum> m_frustum;



};

