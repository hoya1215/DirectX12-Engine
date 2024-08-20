#include "pch.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{

}

void ThreadManager::BeginEvent()
{
	for (auto& thread : m_threads)
	{
		thread.join();
	}
}

void ThreadManager::ClearEvent()
{
	m_threads.clear();
}
