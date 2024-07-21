#include "pch.h"
#include "PipeLine.h"
#include "Mesh.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Object.h"
#include "Camera.h"
#include "CameraMove.h"
#include "DescriptorHeap.h"

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
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	CD3DX12_DESCRIPTOR_RANGE range1[1] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1)
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	CD3DX12_DESCRIPTOR_RANGE range2[1] =
	{
		//CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	slotRootParameter[2].InitAsDescriptorTable(_countof(range2), range2);
	slotRootParameter[1].InitAsDescriptorTable(_countof(range1), range1);
	slotRootParameter[0].InitAsConstantBufferView(0);
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
	CreateShader(m_vertexShader,L"..\\Resources\\Shader\\DefaultVS.hlsl", nullptr, "VS", "vs_5_0");
	CreateShader(m_pixelShader,L"..\\Resources\\Shader\\DefaultVS.hlsl" , nullptr, "PS", "ps_5_0");

	CreateShader(m_skyboxVS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "VS", "vs_5_0");
	CreateShader(m_skyboxPS, L"..\\Resources\\Shader\\Skybox.hlsl", nullptr, "PS", "ps_5_0");

	// inputlayout
	D3D12_INPUT_ELEMENT_DESC ILdesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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
	m_defaultPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
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

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_skyboxPSODesc, IID_PPV_ARGS(&m_skyboxPSO)));
	m_pso.insert({ PSO_TYPE::SKYBOX, m_skyboxPSO });


	// 작업 공간
	WorkSpace();

}


void PipeLine::Render()
{
	//m_cmdList->SetPipelineState(m_defaultPSO.Get());
	//
	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get()};


	// 오류 이유? -> OBJ_HEAP 의 CBV, SRV 두개 다 넣으면 오류
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get() };
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CMD_LIST->SetGraphicsRootSignature(m_rootSignature.Get());

}

void PipeLine::CreateShader(ComPtr<ID3DBlob>& blob, const wstring& filename, const D3D_SHADER_MACRO* defines,
	const string& entrypoint, const string& target)
{
	blob = d3dUtil::CompileShader(filename, defines, entrypoint, target);
}

void PipeLine::CreateSampler()
{
	m_defaultSampler = CD3DX12_STATIC_SAMPLER_DESC(0);
}


void PipeLine::WorkSpace()
{

}

