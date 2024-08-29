#include "pch.h"
#include "KeyInput.h"


void KeyInput::Init()
{
	m_key[Key::W] = "NP";
	m_key[Key::S] = "NP";
	m_key[Key::A] = "NP";
	m_key[Key::D] = "NP";
	m_key[Key::Q] = "NP";
	m_key[Key::E] = "NP";
	m_key[Key::M] = "NP";
}

void KeyInput::Update()
{
	for (auto it = m_key.begin(); it != m_key.end(); it++)
	{
		if (CheckPressed(it->first))
		{
			// 이전에 눌려있었다면
			if (m_key[it->first] == "P" || m_key[it->first] == "KP")
				m_key[it->first] = "KP";
			else
				m_key[it->first] = "P";

		}
		else
			m_key[it->first] = "NP";
	}
}

bool KeyInput::CheckPressed(Key key)
{
	if (GetAsyncKeyState(static_cast<int>(key)) & 0x8000)
	{
		return true;
	}
	else
		return false;
}

string KeyInput::GetKeyState(Key key)
{
	return m_key[key];
}