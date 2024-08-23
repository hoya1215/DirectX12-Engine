#pragma once

#include "Headers.h"
#include <thread>

struct THREAD
{
	thread m_thread{};
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
};

class ThreadManager
{
public:
	ThreadManager(int threadNum = 0);

	void Init();

	template<typename Func, typename Class>
	void AddEvent(Func&& func, Class&& c)
	{

		assert(m_threadIndex < m_threads.size());
		m_threads[m_threadIndex].m_thread = std::thread(
			[func, c, &cmdList = m_threads[m_threadIndex].m_commandList, &cmdQueue =m_commandQueue] {
				(c->*func)(cmdList, cmdQueue);
			}
		);
		m_threadIndex++;
	}

	void ClearCommandList(D3D12_VIEWPORT& viewPort, RECT& rect);

	void BeginEvent();
	void EndEvent();

	void ClearEvent();

	vector<THREAD>& GetThreads() { return m_threads; }
	ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return m_commandQueue; }

public:


	vector<THREAD> m_threads;
	int m_threadIndex = 0;

	ComPtr<ID3D12CommandQueue> m_commandQueue;

	// Final
	ComPtr<ID3D12GraphicsCommandList> m_finalCommandList;
	ComPtr<ID3D12CommandAllocator> m_finalCommandAllocator;
	
};

