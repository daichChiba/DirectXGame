#pragma once
#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include"externals/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// WindowsAPI
class WinApp{
public://メンバ関数
	//初期化
	void Initialize();
	//メッセージの処理
	bool ProcessMessge();
	
	//ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//終了処理
	void Finalize();


	//Getter
	HWND GetHwnd()const { return hwnd; }
	HINSTANCE GetHInstance()const { return wc.hInstance; }
public:
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;
private:
	HWND hwnd = nullptr;
	WNDCLASS wc{};


};

