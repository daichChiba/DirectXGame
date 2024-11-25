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
	/// <param name="winApp">WinAPI</param>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// ディスクリプタヒープを生成する
	/// </summary>
	/// <param name="device"></param>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


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
	void CreateDepthStencilTextureResource();

	/// <summary>
	/// 各種ディスクリプタヒープの初期化
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

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;

	//RTV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap ;
	//SRV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap ;
	//DSV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap ;

};