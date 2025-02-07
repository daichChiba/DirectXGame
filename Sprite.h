#pragma once
#include "MathUtility.h"
#include <cstdint>
#include<wrl.h>

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
	struct Material {
		Vector4 color;
	};
private:
	void CreateVertexData();

private:
	SpriteCommon* spriteCommon_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	//Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//　頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

};

