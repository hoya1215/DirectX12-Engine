#pragma once

enum class Key
{
	W = 'W',
	S = 'S',
	A = 'A',
	D = 'D'
};

enum
{
	KEY_COUNT = 4
};


class KeyInput
{
public:
	void Init();
	void Update();

	bool CheckPressed(Key key);
	string GetKeyState(Key key);

private:
	unordered_map<Key, string> m_key;
};

