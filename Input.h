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
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーを押されたかをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーを押されたかをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>


private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	// 全キーの入力状態を取得する
	BYTE key[256] = {};
};

