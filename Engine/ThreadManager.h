#pragma once

#include "Headers.h"
#include <thread>

struct THREAD
{

};

class ThreadManager
{
public:
	ThreadManager();

	template<typename Func, typename Class>
	void AddEvent(Func&& func, Class&& c)
	{
		m_threads.emplace_back(forward<Func>(func), forward<Class>(c));
	}

	void BeginEvent();

	void ClearEvent();

public:
	vector<thread> m_threads;

	// ¥Ÿ¡ﬂ command queue test
	
};

