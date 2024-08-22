#include "pch.h"
#include "ThreadManager.h"
#include "Engine.h"

ThreadManager::ThreadManager(int threadNum)
{
	m_test.resize(threadNum);
}

void ThreadManager::Init()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// Queue
	ThrowIfFailed(DEVICE->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

	for (int i = 0; i < m_test.size(); ++i)
	{
		ThrowIfFailed(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_test[i].m_commandAllocator.GetAddressOf())));
		ThrowIfFailed(DEVICE->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_test[i].m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_test[i].m_commandList.GetAddressOf())));
	}


}

void ThreadManager::ClearCommandList(D3D12_VIEWPORT& viewPort, RECT& rect)
{
	for (int i = 0; i < m_test.size(); ++i)
	{
		// Reset
		m_test[i].m_commandAllocator->Reset();
		m_test[i].m_commandList->Reset(m_test[i].m_commandAllocator.Get(), nullptr);

		m_test[i].m_commandList->RSSetViewports(1, &viewPort);
		m_test[i].m_commandList->RSSetScissorRects(1, &rect);

		m_test[i].m_commandList->ClearRenderTargetView(
			g_engine->GetCurrentBackBufferHandle(),
			Colors::LightSteelBlue, 0, nullptr);

		m_test[i].m_commandList->ClearDepthStencilView(
			g_engine->GetDepthStencilHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0, 0, nullptr);

	}
}

void ThreadManager::BeginEvent()
{
	//for (auto& thread : m_threads)
	//{
	//	thread.join();
	//}

	for (auto& thread : m_test)
	{
		thread.m_thread.join();
	}
}

void ThreadManager::EndEvent()
{
	vector<ID3D12CommandList*> cmdLists;


	for (int i = 0; i < m_test.size(); ++i)
	{
		ThrowIfFailed(m_test[i].m_commandList->Close());
		cmdLists.push_back(m_test[i].m_commandList.Get());
	}


	m_commandQueue->ExecuteCommandLists(cmdLists.size(), cmdLists.data());
}

void ThreadManager::ClearEvent()
{
	//m_threads.clear();

	m_threadIndex = 0;
}
