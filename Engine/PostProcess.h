#pragma once

class PostProcess
{
public:
	PostProcess();

	void Render(const D3D12_GPU_DESCRIPTOR_HANDLE& rtvGPUHandle);

	void SetFiltersSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE gpuSRVHandle) { m_filtersGPUSRVHandle = gpuSRVHandle; }

public:
	shared_ptr<class Object> m_rectangle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_filtersGPUSRVHandle;

};

