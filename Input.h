#pragma once
#define DIRECTINPUT_VERSION  0x800 //DirectInputのバージョン指定
#include <Windows.h>
#include<wrl.h>
#include<dinput.h>


using namespace Microsoft::WRL;
/// 入力
class Input {
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	/// インスタンスハンドル
	/// ウィンドウハンドル
	void Initialize(HINSTANCE hInstance,HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;


};

