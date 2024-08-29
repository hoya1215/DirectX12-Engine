#pragma once
#include "Component.h"

template <typename T>
class Behavior : public Component<T>
{
public:
	
	virtual void Update() {}
};

