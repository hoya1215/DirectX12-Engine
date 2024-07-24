#pragma once

struct GlobalConstant
{
	Matrix view;
	Matrix proj;
	Matrix viewProj;
	Vector3 eyePos;// World
	
};

struct MeshConstant
{
	Matrix world;
	Matrix worldIT;
	Vector4 pos; 
};