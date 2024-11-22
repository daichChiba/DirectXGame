#include "Input.h"
#include<cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(WinApp* winApp){
	this->winApp_ = winApp;
	HRESULT result;
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update(){
	//前回のキー入力を保存
	memcpy(preKey, key, sizeof(key));

	//DirectX毎フレーム処理
	//キーボード情報の取得開始
	keyboard->Acquire();

	keyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber){
	//指定したキーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber){
	//指定したキーを押していればtrueを返す
	if (key[keyNumber]&&preKey[keyNumber]==0) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::ReleseKey(BYTE keyNumber){
	//指定したキーを押していればtrueを返す
	if (key[keyNumber] == 0 && preKey[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}
