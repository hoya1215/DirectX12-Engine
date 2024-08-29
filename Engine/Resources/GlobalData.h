#pragma once
#include "ConstantBuffer.h"

class GlobalData
{
public:
	void Init();
	void Update();
	void Render(ComPtr<ID3D12GraphicsCommandList>& cmdList);

public:
	void CreateGlobalConstantData();

private:
	// �۷ι� ��� ����
	ComPtr<ID3D12Resource> m_globalConstantBuffer;
	GlobalConstant m_globalConstantData;
	D3D12_GPU_VIRTUAL_ADDRESS m_globalCBAddress;

};

