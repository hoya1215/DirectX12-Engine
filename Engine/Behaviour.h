#pragma once
#include "Component.h"

template <typename T>
class Behaviour : public Component<T>
{
public:
	
	virtual void Update() {}
};

