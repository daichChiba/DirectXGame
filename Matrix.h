#pragma once
#include"math/Vector3.h"
#include"math/Vector4.h"
#include"math/Vector2.h"
#include"math/Matrix4x4.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

Matrix4x4 MakeIdentity4x4();



Matrix4x4 Multply(const Matrix4x4& m1, const Matrix4x4& m2);

// 1.平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 2.拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
// 2.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
// 3.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);


//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//4.逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 1.透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);


// 2.正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// 3.ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float MaxDepth);

Vector3 Cross(const Vector3& v1, const Vector3& v2);
