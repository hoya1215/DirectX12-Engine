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
#include "CommandManager.h"

PipeLine::PipeLine(ComPtr<ID3D12Device>& device)
{
	m_device = device;

}

void PipeLine::Init()
{
	// Root Signature
	CreateSampler();
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	CD3DX12_DESCRIPTOR_RANGE range1[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0)
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	CD3DX12_DESCRIPTOR_RANGE range2[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0)
	};


	CD3DX12_DESCRIPTOR_RANGE range3[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 6)
	};



	//slotRootParameter[6].InitAsShaderResourceView(3);
	//slotRootParameter[5].InitAsShaderResourceView(2);
	//slotRootParameter[4].InitAsShaderResourceView(1);
	slotRootParameter[4].InitAsDescriptorTable(_countof(range3), range3);
	slotRootParameter[3].InitAsDescriptorTable(_countof(range2), range2);
	slotRootParameter[2].InitAsDescriptorTable(_countof(range1), range1);
	slotRootParameter[1].InitAsConstantBufferView(11);
	slotRootParameter[0].InitAsConstantBufferView(10);
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter, 1, &m_defaultSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);



	ComPtr<ID3DBlob> sigBlob;
	ComPtr<ID3DBlob> blobError;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &blobError);
	ThrowIfFailed(DEVICE->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// Compute RootSignature
	CD3DX12_ROOT_PARAMETER computeSlotRootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE crange1[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 0)
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	computeSlotRootParameter[0].InitAsDescriptorTable(_countof(crange1), crange1);
	CD3DX12_ROOT_SIGNATURE_DESC cRootSigDesc(_countof(computeSlotRootParameter), computeSlotRootParameter, 1, &m_defaultSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> cSigBlob;
	ComPtr<ID3DBlob> cBlobError;
	D3D12SerializeRootSignature(&cRootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &cSigBlob, &cBlobError);
	ThrowIfFailed(DEVICE->CreateRootSignature(0, cSigBlob->GetBufferPointer(), cSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_computeRootSignature)));


	//pair<vector<Vertex>, vector<uint16>> meshdata = Mesh::CreateRectangle(0.5f);
	//m_meshBuffer = make_unique<MeshBuffer>();
	//
	//m_meshBuffer->CreateVertexBuffer(meshdata.first, DEVICE);
	//m_meshBuffer->CreateIndexBuffer(meshdata.second, DEVICE);


	// default

	// shader
	CreateAllShader();

	// inputlayout
	D3D12_INPUT_ELEMENT_DESC ILdesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_ELEMENT_DESC ILdesc_Instancing[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},

		{ "WORLDIT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64,  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLDIT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLDIT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "WORLDIT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
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
	m_deferredPSODesc.NumRenderTargets = 5;
	m_deferredPSODesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RTVFormats[4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_deferredPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_deferredPSODesc, IID_PPV_ARGS(&m_deferredPSO)));
	m_pso.insert({ PSO_TYPE::DEFERRED, m_deferredPSO });

	// Deferred_Tessellation
	m_deferredTSPSODesc = m_deferredPSODesc;
	m_deferredTSPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	m_deferredTSPSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	m_deferredTSPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	m_deferredTSPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_deferredTS_VS->GetBufferPointer()),
		m_deferredTS_VS->GetBufferSize()
	};

	m_deferredTSPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_deferredTS_PS->GetBufferPointer()),
		m_deferredTS_PS->GetBufferSize()
	};

	m_deferredTSPSODesc.HS =
	{
		reinterpret_cast<BYTE*>(m_deferredTS_HS->GetBufferPointer()),
		m_deferredTS_HS->GetBufferSize()
	};

	m_deferredTSPSODesc.DS =
	{
		reinterpret_cast<BYTE*>(m_deferredTS_DS->GetBufferPointer()),
		m_deferredTS_DS->GetBufferSize()
	};

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_deferredTSPSODesc, IID_PPV_ARGS(&m_deferredTSPSO)));
	m_pso.insert({ PSO_TYPE::DEFERRED_TS, m_deferredTSPSO });

	// Deferred GS
	m_deferred_GSPSODesc = m_deferredPSODesc;
	m_deferred_GSPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_deferred_GSPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_deferredGS_VS->GetBufferPointer()),
		m_deferredGS_VS->GetBufferSize()
	};

	m_deferred_GSPSODesc.GS =
	{
		reinterpret_cast<BYTE*>(m_deferredGS_GS->GetBufferPointer()),
		m_deferredGS_GS->GetBufferSize()
	};

	m_deferred_GSPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_deferredGS_PS->GetBufferPointer()),
		m_deferredGS_PS->GetBufferSize()
	};

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_deferred_GSPSODesc, IID_PPV_ARGS(&m_deferred_GSPSO)));
	m_pso.insert({ PSO_TYPE::DEFERRED_GS, m_deferred_GSPSO });

	// Deferred Wire
	m_deferredWirePSODesc = m_deferredPSODesc;
	m_deferredWirePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_deferredWirePSODesc, IID_PPV_ARGS(&m_deferredWirePSO)));
	m_pso.insert({ PSO_TYPE::DEFERRED_WIRE, m_deferredWirePSO });

	// Shadow Pass
	m_shadowPassPSODesc = m_defaultPSODesc;
	m_shadowPassPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shadowPassVS->GetBufferPointer()),
		m_shadowPassVS->GetBufferSize()
	};

	m_shadowPassPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shadowPassPS->GetBufferPointer()),
		m_shadowPassPS->GetBufferSize()
	};
	//m_shadowPassPSODesc.RasterizerState.DepthBias = 100000;
	//m_shadowPassPSODesc.RasterizerState.DepthBiasClamp = 0.0f;
	//m_shadowPassPSODesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	//m_shadowPassPSODesc.NumRenderTargets = 0;
	//m_shadowPassPSODesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	//m_shadowPassPSODesc.DepthStencilState.DepthEnable = true;
	//m_shadowPassPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//m_shadowPassPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_shadowPassPSODesc, IID_PPV_ARGS(&m_shadowPassPSO)));
	m_pso.insert({ PSO_TYPE::SHADOW, m_shadowPassPSO });

	// Combine
	m_combinePSODesc = m_defaultPSODesc;
	m_combinePSODesc.VS =
	{
				reinterpret_cast<BYTE*>(m_combineVS->GetBufferPointer()),
		m_combineVS->GetBufferSize()
	};
	m_combinePSODesc.PS =
	{
				reinterpret_cast<BYTE*>(m_combinePS->GetBufferPointer()),
		m_combinePS->GetBufferSize()
	};
	//m_combinePSODesc.DepthStencilState.DepthEnable = false;
	//m_combinePSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_combinePSODesc, IID_PPV_ARGS(&m_combinePSO)));
	m_pso.insert({ PSO_TYPE::COMBINE, m_combinePSO });

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
	//m_postProcessPSODesc.DepthStencilState.DepthEnable = false;
	//m_postProcessPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

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
	m_filterPSODesc.pRootSignature = m_computeRootSignature.Get();
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


void PipeLine::Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	//m_cmdList->SetPipelineState(m_defaultPSO.Get());
	//
	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get()};


	// 오류 이유? -> OBJ_HEAP 의 CBV, SRV 두개 다 넣으면 오류
	//ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get()};
	////ID3D12DescriptorHeap* descriptorHeaps[] = { g_engine->m_deferred->m_deferredSRVHeap.Get()};
	//CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetGraphicsRootSignature(m_rootSignature.Get());
	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::SHADOW)->SetGraphicsRootSignature(m_rootSignature.Get());

	cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
}

//void PipeLine::CreateShader(ComPtr<ID3DBlob>& blob, const wstring& filename, const D3D_SHADER_MACRO* defines,
//	const string& entrypoint, const string& target)
//{
//	blob = d3dUtil::CompileShader(filename, defines, entrypoint, target);
//}

void PipeLine::CreateAllShader()
{


	Util::CreateShader(m_vertexShader, L"..\\Resources\\Shader\\DefaultVS.hlsl", nullptr, "VS", "vs_5_0");


	Util::CreateShader(m_pixelShader, L"..\\Resources\\Shader\\DefaultVS.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_skyboxVS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "VS", "vs_5_0");

	Util::CreateShader(m_skyboxPS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_instancingVS, L"..\\Resources\\Shader\\InstancingVS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_deferredPS, L"..\\Resources\\Shader\\DeferredPS.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_combineVS, L"..\\Resources\\Shader\\Combine.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_combinePS, L"..\\Resources\\Shader\\Combine.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_postProcessVS, L"..\\Resources\\Shader\\PostProcess.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_postProcessPS, L"..\\Resources\\Shader\\PostProcess.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_deferredTS_VS, L"..\\Resources\\Shader\\Deferred_TS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_deferredTS_HS, L"..\\Resources\\Shader\\Deferred_TS.hlsl", nullptr, "HS", "hs_5_0");
	Util::CreateShader(m_deferredTS_DS, L"..\\Resources\\Shader\\Deferred_TS.hlsl", nullptr, "DS", "ds_5_0");
	Util::CreateShader(m_deferredTS_PS, L"..\\Resources\\Shader\\Deferred_TS.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_deferredGS_VS, L"..\\Resources\\Shader\\Deferred_GS.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_deferredGS_GS, L"..\\Resources\\Shader\\Deferred_GS.hlsl", nullptr, "GS", "gs_5_0");
	Util::CreateShader(m_deferredGS_PS, L"..\\Resources\\Shader\\Deferred_GS.hlsl", nullptr, "PS", "ps_5_0");

	//D3D_SHADER_MACRO sm = { "SHADOW", "1" };
	Util::CreateShader(m_shadowPassVS, L"..\\Resources\\Shader\\ShadowPass.hlsl", nullptr, "VS", "vs_5_0");
	Util::CreateShader(m_shadowPassPS, L"..\\Resources\\Shader\\ShadowPass.hlsl", nullptr, "PS", "ps_5_0");

	Util::CreateShader(m_filterCS, L"..\\Resources\\Shader\\FilterCS.hlsl", nullptr, "CS", "cs_5_0");
}

void PipeLine::CreateSampler()
{
	m_defaultSampler = CD3DX12_STATIC_SAMPLER_DESC(0);
}


void PipeLine::WorkSpace()
{

}

