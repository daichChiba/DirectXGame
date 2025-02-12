#include "Enemy.h"
#include "Input.h"

void (Enemy::* Enemy::enemy_[])() = {
	&Enemy::Move,
	&Enemy::Fire,
	&Enemy::Secession
};
void Enemy::Move() {
	ImGui::Text("GoMoooooooooooooooooooooove");
	
}

void Enemy::Fire() {
	ImGui::Text("FireEEEEEEEEEEEEEEEEEEEEEe");
}

void Enemy::Secession() {
	ImGui::Text("secession");
}

void Enemy::Update() {

	Enemys[phase_]();

	//isStart_ = false;
}
