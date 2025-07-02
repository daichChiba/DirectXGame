#include "Sprite.h"
#include "SpriteCommon.h"
#include"DirectXCommon.h"
#include"Transform.h"

using namespace MathUtility;

void Sprite::Initialize(SpriteCommon* spriteCommon) {
	spriteCommon_ = spriteCommon;
	dxCommon_ = spriteCommon->GetDxCommon();
	CreateVertexData();


	// マテリアルリソースを作る
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
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

void Sprite::Updete() {
	//Transform情報を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//TransformからWorldMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//ViewMatrixを作って単位行列を代入
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	//ProjectionMatrixを使って並行行列を書き込む
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	transformtionMatrixData_->WVP = Multply(worldMatrix, Multply(viewMatrix, projectionMatrix));
	transformtionMatrixData_->World = worldMatrix;

}

void Sprite::Draw() {
	//VertexBufferViewを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//IndexBufferViewを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	//描画！(DrawCall/ドローコール)6このインデックスを使用し1つのインスタンスを描画。その他は当面0でよい
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateVertexData() {
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * 4);
	indexResource_ = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
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


	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };// 左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };// 左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };// 右上
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };
	//インデックスリソースにデータを書き込む(6個分)
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}
