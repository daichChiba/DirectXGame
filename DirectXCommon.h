#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include<string>
#include<array>
#include<dxcapi.h>
#include<chrono>
#include<thread>

#include"externals/DirectXTex/DirectXTex.h"
class WinApp;

//DirectX基盤
class DirectXCommon {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">WinAPI</param>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// シェイダーのコンパイル
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath,const wchar_t* profile);

	/// <summary>
	/// バッファリソースの生成
	/// </summary>
	/// <param name="sizeInDytes">データサイズ</param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInDytes);

	/// <summary>
	/// テクスチャリソース
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource( const DirectX::TexMetadata& metadata);
	/// <summary>
	/// テクスチャデータの転送
	/// </summary>
	/// <param name="texture">テクスチャ</param>
	/// <param name="mipImages"></param>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャファイルのパス</param>
	/// <returns>画像イメージデータ</returns>
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//Getter
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap.Get(); }
	ID3D12DescriptorHeap* GetRtvDescriptorHeap() const { return rtvDescriptorHeap.Get(); }
	HANDLE GetFenceEvent() const { return fenceEvent; }

private://メンバ関数


	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void CreateDevice();
	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void CreateCommandRelevance();
	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// 深度バッファ初期化
	/// </summary>
	void CreateDepthStencilTextureResource();

	/// <summary>
	/// 各種ディスクリプタヒープの生成
	/// </summary>
	void CreateAllDescriptorHeap();

	/// <summary>
	/// レンダーターゲットビューの初期化
	/// </summary>
	void RTVInitialize();

	/// <summary>
	/// 深度ステンシルビューの初期化
	/// </summary>
	void DepthStencilInitialize();
	/// <summary>
	/// フェンスの初期化
	/// </summary>
	void CreateFence();
	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void ViewportInitialize();
	/// <summary>
	/// シザリング矩形の生成
	/// </summary>
	void CreateScissorRect();
	/// <summary>
	/// DCXコンパイラの生成
	/// </summary>
	void CreateDxcCompiler();
	/// <summary>
	/// ImGuiの初期化
	/// </summary>
	void ImGuiInitialize();


	/// <summary>
	/// ディスクリプタヒープを生成する
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="heapType">ヒープの種類</param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// 指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap">ディスクリプタヒープ</param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// 指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();
	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

	std::chrono::steady_clock::time_point reference_;

private:

	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判断できる
	HRESULT hr;
	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device>device_;

	//コマンドアロケータを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドキューを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	// スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// スワップチェーンを作成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//RTV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	//SRV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	//DSV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;


	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//スワップチェーンリソース
	std::array< Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	//Fenceを作る
	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};
	//DXCユーティリティ
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	//DXCコンパイラ
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	//デフォルトインクルードハンドラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	//フェンス値
	uint64_t fenceValue = 0;
	//フェンスイベント
	HANDLE fenceEvent;
	//バリア
	D3D12_RESOURCE_BARRIER barrier{};

	

};