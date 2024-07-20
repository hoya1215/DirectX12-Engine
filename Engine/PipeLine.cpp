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

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 2;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	DEVICE->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));

	// Material
	m_test = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(0.f, 0.f, 5.f));
	m_test->GetMaterial()->b_dynamic = true;

	m_test2 = make_shared<Object>(MESH_TYPE::RECTANGLE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(2.f, 0.f, 5.f));
	m_test2->GetMaterial()->b_dynamic = true;

	m_mainCamera = make_shared<Camera>("MainCamera");
	shared_ptr<CameraMove> CM = make_shared<CameraMove>();
	m_mainCamera->AddComponent(COMPONENT_TYPE::BEHAVIOUR, CM);

	// Global Constant
	CreateGlobalConstantData();

	// 작업 공간
	WorkSpace();

}

void PipeLine::Update()
{

	m_mainCamera->Update();

	m_globalConstantData.view = m_mainCamera->m_view;
	m_globalConstantData.proj = m_mainCamera->m_proj;
	m_globalConstantData.viewProj = m_mainCamera->m_view * m_mainCamera->m_proj;


	//Move();
	//Rotate();
	d3dUtil::UpdateConstBuffer(m_globalConstantData, m_globalConstantBuffer);
	m_test->Update();
	m_test2->Update();
	CMD_LIST->SetPipelineState(m_defaultPSO.Get());

}

void PipeLine::Render()
{
	//m_cmdList->SetPipelineState(m_defaultPSO.Get());
	//
	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get()};
	 
	
	// 오류 이유? -> OBJ_HEAP 의 CBV, SRV 두개 다 넣으면 오류
	ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get()};
	CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CMD_LIST->SetGraphicsRootSignature(m_rootSignature.Get());

	//CMD_LIST->IASetVertexBuffers(0, 1, m_meshBuffer->GetVertexBufferView());
	CMD_LIST->IASetVertexBuffers(0, 1, m_test->GetMaterial()->GetMeshBuffer()->GetVertexBufferView());
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
	CMD_LIST->SetGraphicsRootDescriptorTable(1, m_test->GetMaterial()->GetCBVHandle());
	CMD_LIST->SetGraphicsRootDescriptorTable(2, m_test->GetMaterial()->GetSRVHandle());

	CMD_LIST->IASetIndexBuffer(m_test->GetMaterial()->GetMeshBuffer()->GetIndexBufferView());
	CMD_LIST->DrawIndexedInstanced(m_test->GetMaterial()->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);


	// 2
	//CMD_LIST->IASetVertexBuffers(0, 1, m_meshBuffer->GetVertexBufferView());
	CMD_LIST->IASetVertexBuffers(0, 1, m_test2->GetMaterial()->GetMeshBuffer()->GetVertexBufferView());
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
	CMD_LIST->SetGraphicsRootDescriptorTable(1, m_test2->GetMaterial()->GetCBVHandle());
	CMD_LIST->SetGraphicsRootDescriptorTable(2, m_test2->GetMaterial()->GetSRVHandle());

	CMD_LIST->IASetIndexBuffer(m_test2->GetMaterial()->GetMeshBuffer()->GetIndexBufferView());
	CMD_LIST->DrawIndexedInstanced(m_test2->GetMaterial()->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);
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

void PipeLine::WorkSpace()
{

}

