#pragma once

class Object;

class PostProcess
{
public:
	PostProcess();


	void CombineRender(ComPtr<ID3D12GraphicsCommandList>& cmdList, const D3D12_GPU_DESCRIPTOR_HANDLE& rtvGPUHandle);

	void FXAA_Render();

	void PostRender(ComPtr<ID3D12GraphicsCommandList>& cmdList);

	void SetFiltersSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE gpuSRVHandle) { m_filtersGPUSRVHandle = gpuSRVHandle; }

public:
	shared_ptr<Object> m_combineRectangle;
	shared_ptr<Object> m_postProcessRectangle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_filtersGPUSRVHandle;

};

