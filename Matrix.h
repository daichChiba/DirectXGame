#pragma once
#include"math/Vector3.h"
#include"math/Vector4.h"
#include"math/Vector2.h"
#include"math/Matrix4x4.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 ret;
	ret.m[0][0] = 1.0f;
	ret.m[1][1] = 1.0f;
	ret.m[2][2] = 1.0f;
	ret.m[3][3] = 1.0f;
	return ret;
}


Matrix4x4 Multply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 Multply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 ret = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				ret.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return ret;

}
// 1.平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 ret = {};
	ret.m[0][0] = 1.0f;
	ret.m[1][1] = 1.0f;
	ret.m[2][2] = 1.0f;
	ret.m[3][3] = 1.0f;
	ret.m[3][0] = translate.x;
	ret.m[3][1] = translate.y;
	ret.m[3][2] = translate.z;
	return ret;
}
// 2.拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 ret = {};
	ret.m[0][0] = scale.x;
	ret.m[1][1] = scale.y;
	ret.m[2][2] = scale.z;
	ret.m[3][3] = 1.0f;
	return ret;
}

// 1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ret{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,std::cos(radian),std::sin(radian),0.0f,
		0.0f,std::sin(-radian),std::cos(radian),0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	return ret;
}
// 2.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ret{
	std::cos(radian),0.0f,std::sin(-radian),0.0f,
	0.0f,1.0f,0.0f,0.0f,
	std::sin(radian),0.0f,std::cos(radian),0.0f,
	0.0f,0.0f,0.0f,1.0f,
	};
	return ret;

}
// 3.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ret{
		std::cos(radian),std::sin(radian),0.0f,0.0f,
		std::sin(-radian), std::cos(radian),0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	return ret;
}


//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multply(rotateXMatrix, Multply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 ret = Multply(scaleMatrix, Multply(rotateXYZMatrix, translateMatrix));


	return ret;

}

//4.逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 ret;
	//|A|
	float deterlment =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -//1行め
		m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -//2
		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +//3
		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +//4
		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -//5
		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -//6
		m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +//7
		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];//8

	//1/|A|
	float rectDeterlment = 1 / deterlment;
	//m11
	ret.m[0][0] =
		rectDeterlment * (
			m.m[1][1] * m.m[2][2] * m.m[3][3] +
			m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[1][3] * m.m[2][1] * m.m[3][2] -
			m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[1][2] * m.m[2][1] * m.m[3][3] -
			m.m[1][1] * m.m[2][3] * m.m[3][2]);
	//m12
	ret.m[0][1] =
		rectDeterlment * (
			-m.m[0][1] * m.m[2][2] * m.m[3][3] -//
			m.m[0][2] * m.m[2][3] * m.m[3][1] -//
			m.m[0][3] * m.m[2][1] * m.m[3][2] +//1
			m.m[0][3] * m.m[2][2] * m.m[3][1] +//
			m.m[0][2] * m.m[2][1] * m.m[3][3] +//
			m.m[0][1] * m.m[2][3] * m.m[3][2]);//2
	//m13
	ret.m[0][2] =
		rectDeterlment * (
			m.m[0][1] * m.m[1][2] * m.m[3][3] +//
			m.m[0][2] * m.m[1][3] * m.m[3][1] +//
			m.m[0][3] * m.m[1][1] * m.m[3][2] -//1
			m.m[0][3] * m.m[1][2] * m.m[3][1] -//
			m.m[0][2] * m.m[1][1] * m.m[3][3] -//
			m.m[0][1] * m.m[1][3] * m.m[3][2]);//2
	//m14
	ret.m[0][3] =
		rectDeterlment * (
			-m.m[0][1] * m.m[1][2] * m.m[2][3] -//
			m.m[0][2] * m.m[1][3] * m.m[2][1] -//
			m.m[0][3] * m.m[1][1] * m.m[2][2] +//1
			m.m[0][3] * m.m[1][2] * m.m[2][1] +//
			m.m[0][2] * m.m[1][1] * m.m[2][3] +//
			m.m[0][1] * m.m[1][3] * m.m[2][2]);//2

	//m21
	ret.m[1][0] =
		rectDeterlment * (
			-m.m[1][0] * m.m[2][2] * m.m[3][3] -
			m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[1][3] * m.m[2][0] * m.m[3][2] +
			m.m[1][3] * m.m[2][2] * m.m[3][0] +
			m.m[1][2] * m.m[2][0] * m.m[3][3] +
			m.m[1][0] * m.m[2][3] * m.m[3][2]);//2
	//m22
	ret.m[1][1] =
		rectDeterlment * (
			m.m[0][0] * m.m[2][2] * m.m[3][3] +
			m.m[0][2] * m.m[2][3] * m.m[3][0] +
			m.m[0][3] * m.m[2][0] * m.m[3][2] -
			m.m[0][3] * m.m[2][2] * m.m[3][0] -
			m.m[0][2] * m.m[2][0] * m.m[3][3] -
			m.m[0][0] * m.m[2][3] * m.m[3][2]);
	//m23
	ret.m[1][2] =
		rectDeterlment * (
			-m.m[0][0] * m.m[1][2] * m.m[3][3] -
			m.m[0][2] * m.m[1][3] * m.m[3][0] -
			m.m[0][3] * m.m[1][0] * m.m[3][2] +
			m.m[0][3] * m.m[1][2] * m.m[3][0] +
			m.m[0][2] * m.m[1][0] * m.m[3][3] +
			m.m[0][0] * m.m[1][3] * m.m[3][2]);
	//m24
	ret.m[1][3] =
		rectDeterlment * (
			m.m[0][0] * m.m[1][2] * m.m[2][3] +//
			m.m[0][2] * m.m[1][3] * m.m[2][0] +//
			m.m[0][3] * m.m[1][0] * m.m[2][2] -//1
			m.m[0][3] * m.m[1][2] * m.m[2][0] -//
			m.m[0][2] * m.m[1][0] * m.m[2][3] -//
			m.m[0][0] * m.m[1][3] * m.m[2][2]);//2
	//m31
	ret.m[2][0] =
		rectDeterlment * (
			m.m[1][0] * m.m[2][1] * m.m[3][3] +
			m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][1] -
			m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[1][1] * m.m[2][0] * m.m[3][3] -
			m.m[1][0] * m.m[2][3] * m.m[3][1]);
	//m32
	ret.m[2][1] =
		rectDeterlment * (
			-m.m[0][0] * m.m[2][1] * m.m[3][3] -
			m.m[0][1] * m.m[2][3] * m.m[3][0] -
			m.m[0][3] * m.m[2][0] * m.m[3][1] +
			m.m[0][3] * m.m[2][1] * m.m[3][0] +
			m.m[0][1] * m.m[2][0] * m.m[3][3] +
			m.m[0][0] * m.m[2][3] * m.m[3][1]);
	//m33
	ret.m[2][2] =
		rectDeterlment * (
			m.m[0][0] * m.m[1][1] * m.m[3][3] +
			m.m[0][1] * m.m[1][3] * m.m[3][0] +
			m.m[0][3] * m.m[1][0] * m.m[3][1] -
			m.m[0][3] * m.m[1][1] * m.m[3][0] -
			m.m[0][1] * m.m[1][0] * m.m[3][3] -
			m.m[0][0] * m.m[1][3] * m.m[3][1]);
	//m34
	ret.m[2][3] =
		rectDeterlment * (
			-m.m[0][0] * m.m[1][1] * m.m[2][3] -
			m.m[0][1] * m.m[1][3] * m.m[2][0] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] +
			m.m[0][3] * m.m[1][1] * m.m[2][0] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] +
			m.m[0][0] * m.m[1][3] * m.m[2][1]);

	//m41
	ret.m[3][0] =
		rectDeterlment * (
			-m.m[1][0] * m.m[2][1] * m.m[3][2] -
			m.m[1][1] * m.m[2][2] * m.m[3][0] -
			m.m[1][2] * m.m[2][0] * m.m[3][1] +
			m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[1][1] * m.m[2][0] * m.m[3][2] +
			m.m[1][0] * m.m[2][2] * m.m[3][1]);
	//m42
	ret.m[3][1] =
		rectDeterlment * (
			m.m[0][0] * m.m[2][1] * m.m[3][2] +
			m.m[0][1] * m.m[2][2] * m.m[3][0] +
			m.m[0][2] * m.m[2][0] * m.m[3][1] -
			m.m[0][2] * m.m[2][1] * m.m[3][0] -
			m.m[0][1] * m.m[2][0] * m.m[3][2] -
			m.m[0][0] * m.m[2][2] * m.m[3][1]);
	//m43
	ret.m[3][2] =
		rectDeterlment * (
			-m.m[0][0] * m.m[1][1] * m.m[3][2] -
			m.m[0][1] * m.m[1][2] * m.m[3][0] -
			m.m[0][2] * m.m[1][0] * m.m[3][1] +
			m.m[0][2] * m.m[1][1] * m.m[3][0] +
			m.m[0][1] * m.m[1][0] * m.m[3][2] +
			m.m[0][0] * m.m[1][2] * m.m[3][1]);
	//m44
	ret.m[3][3] =
		rectDeterlment * (
			m.m[0][0] * m.m[1][1] * m.m[2][2] +
			m.m[0][1] * m.m[1][2] * m.m[2][0] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] -
			m.m[0][2] * m.m[1][1] * m.m[2][0] -
			m.m[0][1] * m.m[1][0] * m.m[2][2] -
			m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return ret;
}

// 1.透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	float Cot = 1.0f / std::tan(fovY / 2);

	Matrix4x4 ret;
	ret.m[0][0] = (1.0f / aspectRatio) * Cot;
	ret.m[0][1] = 0.0f;
	ret.m[0][2] = 0.0f;
	ret.m[0][3] = 0.0f;

	ret.m[1][0] = 0.0f;
	ret.m[1][1] = Cot;
	ret.m[1][2] = 0.0f;
	ret.m[1][3] = 0.0f;

	ret.m[2][0]	= 0.0f;
	ret.m[2][1]	= 0.0f;
	ret.m[2][2] = farClip / (farClip - nearClip);
	ret.m[2][3] = 1.0f;

	ret.m[3][0] = 0.0f;
	ret.m[3][1] = 0.0f;
	ret.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	ret.m[3][3] = 0.0f;
	return ret;
}

