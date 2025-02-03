#pragma once
#include "MathUtility.h"

//using namespace MathUtility;
class SpriteCommon;
class DirectXCommon;

class Sprite{
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

private:
	SpriteCommon* spriteCommon = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	//Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;
	//　頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
};

