#include "pch.h"
#include "Base.h"
#include "Engine.h"


void Base::Init(const HWND& g_hwnd)
{
	g_engine->Init(g_hwnd);
}



void Base::Render()
{
	g_engine->Render();
}

