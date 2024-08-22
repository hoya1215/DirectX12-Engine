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

		//m_threads.emplace_back(forward<Func>(func), forward<Class>(c));

		assert(m_threadIndex < m_test.size());
		m_test[m_threadIndex].m_thread = std::thread(
			[func, c, &cmdList = m_test[m_threadIndex].m_commandList]() {
				(c->*func)(cmdList);
			}
		);
		m_threadIndex++;
	}

	void ClearCommandList(D3D12_VIEWPORT& viewPort, RECT& rect);

	void BeginEvent();
	void EndEvent();

	void ClearEvent();

	vector<THREAD>& GetThreads() { return m_test; }
	ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return m_commandQueue; }

public:
	vector<thread> m_threads;


	vector<THREAD> m_test;
	int m_threadIndex = 0;

	ComPtr<ID3D12CommandQueue> m_commandQueue;

	// ¥Ÿ¡ﬂ command queue test
	
};

