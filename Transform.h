#pragma once
#include"math/Vector3.h"
struct Transform{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct Particle{
	Transform transform;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};
