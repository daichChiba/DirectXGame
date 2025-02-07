#include "Sprite.h"
#include "SpriteCommon.h"
#include"DirectXCommon.h"
void Sprite::Initialize(SpriteCommon* spriteCommon) {
	spriteCommon_ = spriteCommon;
	dxCommon_ = spriteCommon->GetDxCommon();
	CreateVertexData();


}

void Sprite::CreateVertexData() {
	vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);
	indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);


	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//書き込むためのアドレスを取得(Sprite)
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

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
