#pragma once

class PostProcess
{
public:
	PostProcess();

	void Render(const D3D12_GPU_DESCRIPTOR_HANDLE& rtvGPUHandle);

public:
	shared_ptr<class Object> m_rectangle;
};

