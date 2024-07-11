#include "pch.h"
#include "PipeLine.h"
#include "Mesh.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Material.h"


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
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	CD3DX12_DESCRIPTOR_RANGE range[2] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
	};

	slotRootParameter[1].InitAsDescriptorTable(_countof(range), range);
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




	// shader
	CreateShader(m_vertexShader,L"..\\Resources\\Shader\\DefaultVS.hlsl", nullptr, "VS", "vs_5_0");
	CreateShader(m_pixelShader,L"..\\Resources\\Shader\\DefaultVS.hlsl" , nullptr, "PS", "ps_5_0");

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
	m_defaultPSODesc.DepthStencilState.DepthEnable = FALSE;
	//m_defaultPSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	m_defaultPSODesc.SampleMask = UINT_MAX;
	m_defaultPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_defaultPSODesc.NumRenderTargets = 1;
	m_defaultPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_defaultPSODesc.SampleDesc.Count = 1;
	m_defaultPSODesc.SampleDesc.Quality = 0;
	m_defaultPSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&m_defaultPSODesc, IID_PPV_ARGS(&m_defaultPSO)));

	
	// Global Constant
	CreateGlobalConstantData();


	// Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 2;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	DEVICE->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));

	// Material
	m_material = make_shared<Material>();
	m_material->Init(MESH_TYPE::RECTANGLE, 0.5f, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.jpg", m_cbvHeap);
}

void PipeLine::Update()
{
	Move();
	d3dUtil::UpdateConstBuffer(m_globalConstantData, m_globalConstantBuffer);
	m_material->Update();
	CMD_LIST->SetPipelineState(m_defaultPSO.Get());
}

void PipeLine::Render()
{
	//m_cmdList->SetPipelineState(m_defaultPSO.Get());
	//
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CMD_LIST->SetGraphicsRootSignature(m_rootSignature.Get());

	//CMD_LIST->IASetVertexBuffers(0, 1, m_meshBuffer->GetVertexBufferView());
	CMD_LIST->IASetVertexBuffers(0, 1, m_material->GetMeshBuffer()->GetVertexBufferView());
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
	CMD_LIST->SetGraphicsRootDescriptorTable(1, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	//CMD_LIST->IASetIndexBuffer(m_meshBuffer->GetIndexBufferView());
	CMD_LIST->IASetIndexBuffer(m_material->GetMeshBuffer()->GetIndexBufferView());
	CMD_LIST->DrawIndexedInstanced(m_material->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);
	//CMD_LIST->DrawIndexedInstanced(m_meshBuffer->GetIndexCount(), 1, 0, 0, 0);
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

void PipeLine::CreateGlobalConstantData()
{
	m_globalConstantData.pos = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	d3dUtil::CreateConstBuffer(DEVICE, m_globalConstantData, m_globalConstantBuffer, 1);
	m_globalCBAddress = m_globalConstantBuffer->GetGPUVirtualAddress();
}

void PipeLine::Move()
{
	if (INPUT->GetKeyState(Key::W) == "P" || INPUT->GetKeyState(Key::W) == "KP")
	{
		m_globalConstantData.pos.y += 1.f * DT;

	}

	if (INPUT->GetKeyState(Key::S) == "P" || INPUT->GetKeyState(Key::S) == "KP")
	{
		m_globalConstantData.pos.y -= 1.f * DT;
	}

	if (INPUT->GetKeyState(Key::D) == "P" || INPUT->GetKeyState(Key::D) == "KP")
	{
		m_globalConstantData.pos.x += 1.f * DT;
	}

	if (INPUT->GetKeyState(Key::A) == "P" || INPUT->GetKeyState(Key::A) == "KP")
	{
		m_globalConstantData.pos.x -= 1.f * DT;
	}

}
