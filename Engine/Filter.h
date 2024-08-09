#pragma once


class Filter
{
public:
	Filter();
	void Init(COMPUTE_PSO_TYPE computePSOType);

	void Render();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle() { return m_srvGPUHandle; }

private:
	ComPtr<ID3D12Resource> m_buffer;

	D3D12_CPU_DESCRIPTOR_HANDLE m_srvCPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_uavCPUHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_srvGPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_uavGPUHandle;

	ComPtr<ID3DBlob> m_filterCS;

	COMPUTE_PSO_TYPE m_computePSOType;
};

