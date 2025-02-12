#pragma once
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include"externals/DirectXTex/DirectXTex.h"
#include "Input.h"
#include<unordered_map>
#include <functional>

typedef void(*PFunc)();

enum class Phase{
	Move,
	Fire,
	Secession
};
/// <summary>
/// 敵
/// </summary>
class Enemy {
public:
	void Move();
	void Fire();
	void Secession();
	void Update();
	void SetStart(bool isStart) {isStart_ = isStart;}
	bool GetStart() { return isStart_; }
	void ChangePhase() {
		phase_ = nextPhase[phase_];
	}
private:
	Phase phase_ = Phase::Move;

	std::unordered_map<Phase, std::function<void()>> Enemys = {
		{Phase::Move,[this]() {Move(); }},
		{Phase::Fire, [this]() {Fire(); }},
		{Phase::Secession, [this]() {Secession(); }}
	};
	std::unordered_map<Phase, Phase> nextPhase = {
		{Phase::Move,Phase::Fire},
		{Phase::Fire,Phase::Secession},
		{Phase::Secession,Phase::Move}
	};

	//メンバ関数ポインタ
	static void (Enemy::* enemy_[])();
	bool isStart_ = false;
};