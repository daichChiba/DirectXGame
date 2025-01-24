#pragma once
#include"math/Vector3.h"
struct Transform{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float shininess;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;///!<ライトの色
	Vector3 direction;///!<ライトの向き
	float intensity;///!<輝度
};

struct CameraForGPU {
	Vector3 worldPosition;
};