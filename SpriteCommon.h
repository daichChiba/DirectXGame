#pragma once
#include<d3d12.h>
class DirectXCommon;
class SpriteCommon {
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

public://メンバ変数
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	void PreDraw();

private://メンバ初期化関数
	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();
	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void CreateGraphicsPipeline();

private://メンバ生成関数


private:
	//ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//DirectXCommon
	DirectXCommon* dxCommon_;

};

