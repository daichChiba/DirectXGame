#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include<array>
#include<dxcapi.h>
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

private:


	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void DeviceInitialize();
	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void CommandRelevanceInitialize();
	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void SwapChainInitialize();

	/// <summary>
	/// 深度バッファ初期化
	/// </summary>
	void CreateDepthStencilTextureResource();

	/// <summary>
	/// 各種ディスクリプタヒープの生成
	/// </summary>
	void AllDescriptorHeapInitialize();

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
	void FenceInitialize();
	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void ViewportInitialize();
	/// <summary>
	/// シザリング矩形の生成
	/// </summary>
	void ScissorRectInitialize();
	/// <summary>
	/// DCXコンパイラの生成
	/// </summary>
	void DxcCompilerInitialize();
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
	/// <param name="descriptorHeap"></param>
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


private:

	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判断できる
	HRESULT hr;
	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device>device;
	//コマンドアロケータを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキューを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	// スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// スワップチェーンを作成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

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


};