#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include <vector>

#define SCR_WIDTH    (1920)
#define SCR_HEIGHT	 (1080)
#define ASPECT_RATIO ((float)SCR_WIDTH/(float)SCR_HEIGHT)
#define NEAR_PLANE   (0.1f)
#define FAR_PLANE    (100.0f)

//�@�����ݒ�
#define YAW     (-90.0f)
#define PITCH   (0.0f)
#define ROLL    (0.0f)
#define SPEED   (5.0f)
#define SENSI   (0.1f)
#define ZOOM    (45.0f)
#define R_ANGLE (90.0f)

// �J�����ړ�����
enum Camera_Movement {
	C_FORWARD,
	C_BACKWARD,
	C_LEFT,
	C_RIGHT
};

enum Camera_Mode
{
	FREE,
	FIRST_PERSON,
	THIRD_PERSON,
};

class Camera
{
public:
	// �J��������
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// �I�C���[�p
	float Yaw;
	float Pitch;
	float Roll;
	// �ړ��֘A�p�����[�^
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

public:
	// �R���X�g���N�^ ���F�N�g��ver
	Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL);

	// �R���X�g���N�^ �X�J���[ver
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll);

	void BindCamera(Shader& shader);

	// view matrix ���擾
	inline glm::mat4 GetViewMatrix()
	{
		if (m_Mode == FREE)
			return My_LookAt_Matrix(Position, Position + Front, Up);
		else 
		{
			glm::vec3 focus = *m_Focus;
			focus.y += 0.5f;
			return My_LookAt_Matrix(Position, focus, Up);
		}

	}

	// Keyboard���͏���
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	// mouse���͏���
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// mouse�X�N���[���̍X�V����
	void ProcessMouseScroll(float yoffset);
	void LoadFoucs(glm::vec3* focus)
	{
		m_Focus = focus;
		m_Len = glm::length(*focus - Position);
	}
	// ���� LookAt function
	glm::mat4 My_LookAt_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);

	bool* GetLock() { return &m_Lock; }
	inline void updateCameraVectorsThird()
	{

		// Front vector���v�Z
		Position.x = m_Focus->x - glm::sin(glm::radians(Yaw + YAW)) * m_Len;
		Position.z = m_Focus->z - glm::cos(glm::radians(Yaw + YAW)) * m_Len;
		Position.y = (m_Focus->y + 0.5f) + glm::tan(glm::radians(Pitch + PITCH)) * m_Len;
		glm::vec3 front;
		glm::vec3 focus = *m_Focus;
		focus.y += 0.5f;
		front = focus - Position;
		Front = glm::normalize(front);
		// Right and Up vector�@���v�Z
		Right = glm::normalize(glm::cross(Front, WorldUp));  // ���K��
		Up = glm::normalize(glm::cross(Right, Front));
	}
private:
	bool m_Lock;        //�G�l�~�[���b�N���H
	Camera_Mode m_Mode; //�J�������[�h
	glm::vec3* m_Focus; //�����ڕW���W
	float m_Len;        //�����ڕW�Ƃ̋���
private:

	inline void updateCameraVectorsFree()
	{
		// Front vector���v�Z
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Right and Up vector�@���v�Z
		Right = glm::normalize(glm::cross(Front, WorldUp));  // ���K��
		Up = glm::normalize(glm::cross(Right, Front));
	}


};

#endif