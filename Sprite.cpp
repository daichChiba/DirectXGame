#include "Sprite.h"
#include "SpriteCommon.h"
#include"DirectXCommon.h"

using namespace MathUtility;

void Sprite::Initialize(SpriteCommon* spriteCommon) {
	spriteCommon_ = spriteCommon;
	dxCommon_ = spriteCommon->GetDxCommon();
	CreateVertexData();


	// マテリアルリソースを作る
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Vector4));
	// マテリアルデータの初期値を書き込む
	materialData->color = { 1.0f,1.0f,1.0f,1.0f }; // 白色
	materialData->enableLighting = false; // ライティングを無効化
	materialData->uvTransform = MakeIdentity4x4();

	//座標変換行列リソースを作る
	transformationMatrixResource_ = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));
	//座標変換行列リソースに書き込むためのアドレスを取得してtransformationMatrixData_に割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformtionMatrixData_));
	//単位行列を書き込んでおく
	transformtionMatrixData_->WVP = MakeIdentity4x4();
	transformtionMatrixData_->World = MakeIdentity4x4();

}

void Sprite::Updete(){


}

void Sprite::CreateVertexData() {
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);
	indexResource_ = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);




	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//書き込むためのアドレスを取得(Sprite)
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	//1枚目の三角形
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData[3].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData[3].texcoord = { 0.0f,0.0f };
	vertexData[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexData[4].texcoord = { 1.0f,0.0f };
	vertexData[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[5].texcoord = { 1.0f,1.0f };

}
