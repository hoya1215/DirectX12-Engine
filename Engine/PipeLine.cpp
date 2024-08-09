#include "pch.h"
#include "PipeLine.h"
#include "Mesh.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Object.h"
#include "Camera.h"
#include "CameraMove.h"
#include "DescriptorHeap.h"
#include "RenderTargets.h"
#include "Util.h"

PipeLine::PipeLine(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12GraphicsCommandList>& resourceCmdList)
{
	m_device = device;
	m_cmdList = cmdList;
	m_resCmdList = resourceCmdList;
}

void PipeLine::Init()
{
	// Root Signature
	CreateSampler();
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	CD3DX12_DESCRIPTOR_RANGE range1[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0)
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	CD3DX12_DESCRIPTOR_RANGE range2[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0)
	};

	CD3DX12_DESCRIPTOR_RANGE range3[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 3, 0)
	};

	CD3DX12_DESCRIPTOR_RANGE range4[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 3)
	};



	//slotRootParameter[6].InitAsShaderResourceView(3);
	//slotRootParameter[5].InitAsShaderResourceView(2);
	//slotRootParameter[4].InitAsShaderResourceView(1);
	slotRootParameter[5].InitAsDescriptorTable(_countof(range4), range4);
	slotRootParameter[4].InitAsDescriptorTable(_countof(range3), range3);
	slotRootParameter[3].InitAsDescriptorTable(_countof(range2), range2);
	slotRootParameter[2].InitAsDescriptorTable(_countof(range1), range1);
	slotRootParameter[1].InitAsConstantBufferView(11);
	slotRootParameter[0].InitAsConstantBufferView(10);
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter, 1, &m_defaultSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);



	ComPtr<ID3DBlob> sigBlob;
	ComPtr<ID3DBlob> blobError;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &blobError);
	DEVICE->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));


	pair<vector<Vertex>, vector<uint16>> meshdata = Mesh::CreateRectangle(0.5f);
	m_meshBuffer = make_unique<MeshBuffer>();

	m_meshBuffer->CreateVertexBuffer(meshdata.first, DEVICE);
	m_meshBuffer->CreateIndexBuffer(meshdata.second, DEVICE);


	// default

	// shader
	Util::CreateShader(m_vertexShader,L"..\\Resources\\Shader\\DefaultVS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_pixelShader,L"..\\Resources\\Shader\\DefaultVS.hlsl" , nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_skyboxVS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_skyboxPS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_instancingVS, L"..\\Resources\\Shader\\InstancingVS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_deferredPS, L"..\\Resources\\Shader\\DeferredPS.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_postProcessVS, L"..\\Resources\\Shader\\PostProcessPS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_postProcessPS, L"..\\Resources\\Shader\\PostProcessPS.hlsl", nullptr, "PS", "ps_5_0");
	
	Util::CreateShader(m_filterCS, L"..\\Resources\\Shader\\FilterCS.hlsl", nullptr, "CS", "cs_5_0");

	// inputlayout
	D3D12_INPUT_ELEMENT_DESC ILdesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_ELEMENT_DESC ILdesc_Instancing[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
	};

	// pipeline
	m_defaultPSODesc.InputLayout = { ILdesc, _countof(ILdesc)};
	m_defaultPSODesc.pRootSignature = m_rootSignature.Get();
	m_defaultPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_vertexShader->GetBufferPointer()),
		m_vertexShader->GetBufferSize()
	};
	m_defaultPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_pixelShader->GetBufferPointer()),
		m_pixelShader->GetBufferSize()
	};
	m_defaultPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_defaultPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//m_defaultPSODesc.DepthStencilState.DepthEnable = FALSE;
	m_defaultPSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	m_defaultPSODesc.SampleMask = UINT_MAX;
	m_defaultPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_defaultPSODesc.NumRenderTargets = 1;
	m_defaultPSODesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_defaultPSODesc.SampleDesc.Count = 1;
	m_defaultPSODesc.SampleDesc.Quality = 0;
	m_defaultPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_defaultPSODesc, IID_PPV_ARGS(&m_defaultPSO)));
	m_pso.insert({ PSO_TYPE::DEFAULT, m_defaultPSO });

	// skybox
	m_skyboxPSODesc = m_defaultPSODesc;
	m_skyboxPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_skyboxVS->GetBufferPointer()),
		m_skyboxVS->GetBufferSize()
	};
	m_skyboxPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_skyboxPS->GetBufferPointer()),
		m_skyboxPS->GetBufferSize()
	};
	m_skyboxPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	m_skyboxPSODesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_skyboxPSODesc, IID_PPV_ARGS(&m_skyboxPSO)));
	m_pso.insert({ PSO_TYPE::SKYBOX, m_skyboxPSO });

	// Deferred
	m_deferredPSODesc = m_defaultPSODesc;
	m_deferredPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_deferredPS->GetBufferPointer()),
		m_deferredPS->GetBufferSize()
	};
	m_deferredPSODesc.NumRenderTargets = 3;
	m_deferredPSODesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_deferredPSODesc, IID_PPV_ARGS(&m_deferredPSO)));
	m_pso.insert({ PSO_TYPE::DEFERRED, m_deferredPSO });


	// PostProcess
	m_postProcessPSODesc = m_defaultPSODesc;
	m_postProcessPSODesc.VS =
	{
				reinterpret_cast<BYTE*>(m_postProcessVS->GetBufferPointer()),
		m_postProcessVS->GetBufferSize()
	};
	m_postProcessPSODesc.PS =
	{
				reinterpret_cast<BYTE*>(m_postProcessPS->GetBufferPointer()),
		m_postProcessPS->GetBufferSize()
	};
	m_postProcessPSODesc.DepthStencilState.DepthEnable = false;
	m_postProcessPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_postProcessPSODesc, IID_PPV_ARGS(&m_postProcessPSO)));
	m_pso.insert({ PSO_TYPE::POST_PROCESS, m_postProcessPSO });

	// Instancing
	m_instancingPSODesc = m_deferredPSODesc;
	m_instancingPSODesc.InputLayout = { ILdesc_Instancing, _countof(ILdesc_Instancing) };
	m_instancingPSODesc.VS =
	{
				reinterpret_cast<BYTE*>(m_instancingVS->GetBufferPointer()),
		m_instancingVS->GetBufferSize()
	};

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_instancingPSODesc, IID_PPV_ARGS(&m_instancingPSO)));
	m_pso.insert({ PSO_TYPE::INSTANCING, m_instancingPSO });

	// Filter
	m_filterPSODesc.pRootSignature = m_rootSignature.Get();
	m_filterPSODesc.CS =
	{
		reinterpret_cast<BYTE*>(m_filterCS->GetBufferPointer()),
		m_filterCS->GetBufferSize()
	};

	ThrowIfFailed(DEVICE->CreateComputePipelineState(&m_filterPSODesc, IID_PPV_ARGS(&m_filterPSO)));
	m_computePSO.insert({ COMPUTE_PSO_TYPE::FILTER, m_filterPSO });

	// 작업 공간
	WorkSpace();

}


void PipeLine::Render()
{
	//m_cmdList->SetPipelineState(m_defaultPSO.Get());
	//
	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get()};


	// 오류 이유? -> OBJ_HEAP 의 CBV, SRV 두개 다 넣으면 오류
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get()};
	//ID3D12DescriptorHeap* descriptorHeaps[] = { g_engine->m_deferred->m_deferredSRVHeap.Get()};
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CMD_LIST->SetGraphicsRootSignature(m_rootSignature.Get());

}

//void PipeLine::CreateShader(ComPtr<ID3DBlob>& blob, const wstring& filename, const D3D_SHADER_MACRO* defines,
//	const string& entrypoint, const string& target)
//{
//	blob = d3dUtil::CompileShader(filename, defines, entrypoint, target);
//}

void PipeLine::CreateSampler()
{
	m_defaultSampler = CD3DX12_STATIC_SAMPLER_DESC(0);
}


void PipeLine::WorkSpace()
{

}

