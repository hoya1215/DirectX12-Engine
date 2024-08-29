#include "pch.h"
#include "CommandManager.h"
#include "Engine.h"

CommandManager::CommandManager()
{
}

void CommandManager::Create()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// Queue
	ThrowIfFailed(DEVICE->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(m_finalCommandQueue.GetAddressOf())));


	// Allocator
	shared_ptr<COMMANDLIST_DESC> main = make_shared<COMMANDLIST_DESC>();

	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(main->m_commandAllocator.GetAddressOf())));
	ThrowIfFailed(DEVICE->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		main->m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(main->m_commandList.GetAddressOf())));
	main->m_renderOrder = 1;

	m_commandLists.insert({ COMMANDLIST_TYPE::MAIN, main });

	
	shared_ptr<COMMANDLIST_DESC> shadow = make_shared<COMMANDLIST_DESC>();

	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(shadow->m_commandAllocator.GetAddressOf())));
	ThrowIfFailed(DEVICE->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		shadow->m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(shadow->m_commandList.GetAddressOf())));
	shadow->m_renderOrder = 0;

	m_commandLists.insert({ COMMANDLIST_TYPE::SHADOW, shadow });

	// Resource
	ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_resourceCmdAllocator.GetAddressOf())));
	ThrowIfFailed(DEVICE->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_resourceCmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_resourceCmdList.GetAddressOf())));




	for (auto it = m_commandLists.begin(); it != m_commandLists.end(); ++it)
	{
		it->second->m_commandList->Close();
	}

}

void CommandManager::Reset()
{
	for (auto it = m_commandLists.begin(); it != m_commandLists.end(); ++it)
	{
		it->second->m_commandAllocator->Reset();
		it->second->m_commandList->Reset(it->second->m_commandAllocator.Get(), nullptr);
	}
}

void CommandManager::SetViewPort(D3D12_VIEWPORT& viewPort, RECT& rect)
{
	for (auto it = m_commandLists.begin(); it != m_commandLists.end(); ++it)
	{
		it->second->m_commandList->RSSetViewports(1, &viewPort);
		it->second->m_commandList->RSSetScissorRects(1, &rect);
	}			  
}


void CommandManager::Clear()
{
	for (auto it = m_commandLists.begin(); it != m_commandLists.end(); ++it)
	{
		it->second->m_commandList->ClearRenderTargetView(
			g_engine->GetCurrentBackBufferHandle(),
			Colors::LightSteelBlue, 0, nullptr);

		it->second->m_commandList->ClearDepthStencilView(
			g_engine->GetDepthStencilHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0, 0, nullptr);
	}
}

void CommandManager::Close()
{
	for (auto it = m_commandLists.begin(); it != m_commandLists.end(); ++it)
	{
		ThrowIfFailed(it->second->m_commandList->Close());
	}
}


void CommandManager::ExecuteCommandLists()
{
	// юс╫ц

	ID3D12CommandList* cmdLists[] = { GetCmdList(COMMANDLIST_TYPE::SHADOW).Get(),
		GetCmdList(COMMANDLIST_TYPE::MAIN).Get()};

	m_finalCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

