#pragma once

#include "pch.h"

class Engine;

class Base
{
public:
	void Init(const HWND& g_hwnd);
	void Render();

private:
	//unique_ptr<Engine> g_engine = make_unique<Engine>();

};

