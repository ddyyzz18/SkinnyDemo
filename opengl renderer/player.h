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
//���͕���
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
	Model* m_Model;					//�g�p���郂�f��
	std::vector<Animation*> m_Moves;//�֘A����A�j�}�V�I���̃|�C���g�@�x�N�g��
	std::vector<Blender*> m_Blender;//�֘A����u�����_�[�̃|�C���g�@�x�N�g��
	PlayerMove m_Move;				//���Đ����Ă�A�j�}�V�I���̔ԍ�
	bool m_Use;						//�g�p���t���O

	float m_MovementSpeed;			//�v���C���[�@�X�s�[�h
	glm::vec3 m_Front;				//�O�x�N�g��
	glm::vec3 m_Right;				//�E�x�N�g��
private:
	void UpdateAnimation(float dt);
	void ProcessTurnAround(int* count);
public:
	Animator m_Animator;			//�A�j�}�V�I������p�A�j���^�[
	bool m_Unable;					//�u�����h�֎~���

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