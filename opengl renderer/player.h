#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "gameobject.h"
#include "model_animation.h"
#include "animation.h"
#include "animator.h"
#include "keyboard.h"
#include "camera.h"
#include <vector>

#define MAX_SPEED (6.0f)
#define WALK_SPEED (3.0f)
#define ACC (5.0f)
//入力方向
enum InputState
{
	P_NONE,
	P_FORWARD,
	P_BACKWARD,
	P_LEFT,
	P_RIGHT,
	P_LOCK,

	P_INPUT_MAX
};

enum PlayerMove
{
	IDLE,
	IDLE_2,
	WALK_FRONT,
	RUN_FRONT,
	WALK_BACK,
	RUN_BACK,
	U_TURN,
	LEFT_TURN,
	RIGHT_TURN,
	ATTACK1,
	BLOCK,
	DODGE,
	JUMP_ATT,
	ZERO_IDLE,
	STATIC_IDLE,
	
	MOVE_MAX

};

class Player:public GameObject
{
private:
	Model* m_Model;					//使用するモデル
	std::vector<Animation*> m_Moves;//関連するアニマシオンのポイント　ベクトル
	std::vector<Blender*> m_Blender;//関連するブレンダーのポイント　ベクトル
	PlayerMove m_Move;				//今再生してるアニマシオンの番号
	bool m_Use;						//使用中フラグ

	float m_MovementSpeed;			//プレイヤー　スピード
	glm::vec3 m_Front;				//前ベクトル
	glm::vec3 m_Right;				//右ベクトル
private:
	void UpdateAnimation(float dt);
	void ProcessTurnAround(int* count);
public:
	Animator m_Animator;			//アニマシオン制御用アニメター
	bool m_Unable;					//ブレンド禁止状態

public:
	Player(Model* model, std::vector <Animation*>& ani);
	Player();
	Player(Player&& player) noexcept;
	~Player(){};
	void LoadAnimation(Animation*);
	void LoadModel(Model* model);
	void LoadBlender(Blender* blender);
	void SetupAnimator();
	void Update(float dt);
	void Draw(Shader& shader);
	void BindFinalBoneMatrices(Shader& shader);
	void BindWorldMatrix(Shader& shader);
	void ProcessKeyboard(InputState in, float dt, Camera& camera);
	void Reset();
	void StartBlend(PlayerMove move);
	void StartBlend(Blender*,int);
	void PlayAni(PlayerMove move);
	void UpdatePlayerVectors(Camera& camera);
	inline int GetAniIndex() { return static_cast<int>(m_Move); }
	inline Animation* RetrieveAni(int index) { return m_Moves[index]; }
};

#endif