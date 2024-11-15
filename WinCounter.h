#pragma once
#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include"externals/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// WindowsAPI
class WinCounter{
public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	WNDCLASS wc{};
	HWND hwnd;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:


};

