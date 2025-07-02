#pragma once
#include "MathUtility.h"
#include <cstdint>
#include<wrl.h>
#include"externals/DirectXTex/d3dx12.h"

//using namespace MathUtility;
class SpriteCommon;
class DirectXCommon;

class Sprite{
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Updete();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

private:
	void CreateVertexData();

private:
	SpriteCommon* spriteCommon_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	//Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	// バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformtionMatrixData_ = nullptr;

	//　Sprite用のインデックスリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//　頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

};

