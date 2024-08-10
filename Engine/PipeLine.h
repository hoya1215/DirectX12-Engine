#pragma once


#include "pch.h"
#include "MeshData.h"
#include "MeshBuffer.h"
#include "Texture.h"
#include "ConstantBuffer.h"

enum
{
	CBV_REGISTER_INDEX = 0,
	SRV_REGISTER_INDEX = 1,
	CBV_REGISTER_COUNT = 1,
	SRV_REGISTER_COUNT = 1,
	DESCRIPTOR_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT

};

class Object;
class Camera;

class PipeLine
{

public:
	PipeLine(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12GraphicsCommandList>& resourceCmdList);

	// 파이프라인
	void Init();
	void WorkSpace();
	void Render();


	// 생성
	void CreateAllShader();

	void CreateSampler();
	void CreateGlobalConstantData();

	ComPtr<ID3D12PipelineState> GetPSO(PSO_TYPE psoType) { return m_pso[psoType]; }
	ComPtr<ID3D12PipelineState> GetComputePSO(COMPUTE_PSO_TYPE computePSOType) { return m_computePSO[computePSOType]; }
	ComPtr<ID3D12RootSignature> GetRootSignature() { return m_rootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return m_computeRootSignature; }

private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12GraphicsCommandList> m_resCmdList;

	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12RootSignature> m_computeRootSignature;

	D3D12_STATIC_SAMPLER_DESC m_defaultSampler;

	// Mesh
	unique_ptr<MeshBuffer> m_meshBuffer;


	// shader
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ComPtr<ID3DBlob> m_skyboxVS;
	ComPtr<ID3DBlob> m_skyboxPS;
	ComPtr<ID3DBlob> m_instancingVS;
	ComPtr<ID3DBlob> m_deferredPS;
	ComPtr<ID3DBlob> m_postProcessVS;
	ComPtr<ID3DBlob> m_postProcessPS;

	ComPtr<ID3DBlob> m_deferredTS_VS;
	ComPtr<ID3DBlob> m_deferredTS_HS;
	ComPtr<ID3DBlob> m_deferredTS_DS;
	ComPtr<ID3DBlob> m_deferredTS_PS;


	ComPtr<ID3DBlob> m_filterCS;


	// pipeline
	ComPtr<ID3D12PipelineState> m_defaultPSO;
	ComPtr<ID3D12PipelineState> m_skyboxPSO;
	ComPtr<ID3D12PipelineState> m_deferredPSO;
	ComPtr<ID3D12PipelineState> m_deferredWirePSO;
	ComPtr<ID3D12PipelineState> m_deferredTSPSO; // HS , DS 포함한 테셀레이션
	ComPtr<ID3D12PipelineState> m_instancingPSO; // deferred
	ComPtr<ID3D12PipelineState> m_postProcessPSO;
	ComPtr<ID3D12PipelineState> m_filterPSO;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_defaultPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_skyboxPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_deferredPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_deferredWirePSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_deferredTSPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_postProcessPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_instancingPSODesc = {};

	D3D12_COMPUTE_PIPELINE_STATE_DESC m_filterPSODesc = {};



	// PSO Map
	unordered_map<PSO_TYPE, ComPtr<ID3D12PipelineState>> m_pso;
	unordered_map<COMPUTE_PSO_TYPE, ComPtr<ID3D12PipelineState>> m_computePSO;
};

