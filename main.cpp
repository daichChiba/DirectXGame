#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg){
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//　標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message) {
	OutputDebugStringA(ConvertString(message).c_str());
}





//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!");

	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラスを登録する
	RegisterClass(&wc);

	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//利用するクラス名
		L"CG2",					//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標（windowsに任せる）
		CW_USEDEFAULT,			//表示Y座標（windowsOSに任せる）
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//縦ウィンドウハンドル
		nullptr,				//メニューハンドル
		wc.hInstance,			//インスタンスハンドル
		nullptr					//オプション
	);

	//文字列を格納する
	std::string str0{ "STRING!!!" };

	//整数を文字列にする
	std::string str1{ std::to_string(10) };



	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
	//ここから下に05の資料を書いていく
	
	//DXGIファクトリーの作成
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判断できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期化の根本的な問題でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
		i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&useAdapter))!=
		DXGI_ERROR_NOT_FOUND; i++){
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); //取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags&DXGI_ADAPTER_FLAG3_SOFTWARE)){
			//採用したアダプタの情報をログに出力。wstringのほうなので注意
			Log(std::format(L"Use Adapater;{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr;
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); i++){
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)){
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel ] {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");//初期化完了ログを出す



	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message!=WM_QUIT){
		//windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else{
			//ゲームの処理

		}
	}

	return 0;
}