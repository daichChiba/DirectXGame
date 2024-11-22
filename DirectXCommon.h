#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
class WinApp;

//DirectX基盤
class DirectXCommon {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);


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
	/// スワップチェーンの初期化
	/// </summary>
	void SwapChainInitialize();

	/// <summary>
	/// 深度バッファ初期化
	/// </summary>
	void AllDescriptorHeapInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void RTVInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void DepthStencilInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void FenceInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void ViewportInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void ScissorRectInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void DxcCompilerInitialize();
	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void ImGuiInitialize();


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

	// スワップチェーンを作成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};