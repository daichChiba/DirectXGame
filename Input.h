#pragma once
#define DIRECTINPUT_VERSION  0x800 //DirectInputのバージョン指定
#include <Windows.h>
#include<wrl.h>
#include<dinput.h>
#include"WinApp.h"

using namespace Microsoft::WRL;
/// 入力
class Input {
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">windowsAPIの変数</param>
	void Initialize(WinApp* winApp);

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
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーを一度だけ押されたかチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>一度だけ押されたか</returns>
	bool ReleseKey(BYTE keyNumber);

private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	// DirectInputのインスタンス生成
	ComPtr<IDirectInput8> directInput = nullptr;


	// 全キーの入力状態を取得する
	BYTE key[256] = {};

	//前回の全キーの入力状態を取得する
	BYTE preKey[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

