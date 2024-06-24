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

//　初期設定
#define YAW     (-90.0f)
#define PITCH   (0.0f)
#define ROLL    (0.0f)
#define SPEED   (5.0f)
#define SENSI   (0.1f)
#define ZOOM    (45.0f)
#define R_ANGLE (90.0f)

// カメラ移動方向
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
	// カメラ属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// オイラー角
	float Yaw;
	float Pitch;
	float Roll;
	// 移動関連パラメータ
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

public:
	// コンストラクタ ヴェクトルver
	Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL);

	// コンストラクタ スカラーver
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll);

	void BindCamera(Shader& shader);

	// view matrix を取得
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

	// Keyboard入力処理
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	// mouse入力処理
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// mouseスクロールの更新処理
	void ProcessMouseScroll(float yoffset);
	void LoadFoucs(glm::vec3* focus)
	{
		m_Focus = focus;
		m_Len = glm::length(*focus - Position);
	}
	// 自作 LookAt function
	glm::mat4 My_LookAt_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);

	bool* GetLock() { return &m_Lock; }
	inline void updateCameraVectorsThird()
	{

		// Front vectorを計算
		Position.x = m_Focus->x - glm::sin(glm::radians(Yaw + YAW)) * m_Len;
		Position.z = m_Focus->z - glm::cos(glm::radians(Yaw + YAW)) * m_Len;
		Position.y = (m_Focus->y + 0.5f) + glm::tan(glm::radians(Pitch + PITCH)) * m_Len;
		glm::vec3 front;
		glm::vec3 focus = *m_Focus;
		focus.y += 0.5f;
		front = focus - Position;
		Front = glm::normalize(front);
		// Right and Up vector　を計算
		Right = glm::normalize(glm::cross(Front, WorldUp));  // 正規化
		Up = glm::normalize(glm::cross(Right, Front));
	}
private:
	bool m_Lock;        //エネミーロック中？
	Camera_Mode m_Mode; //カメラモード
	glm::vec3* m_Focus; //注視目標座標
	float m_Len;        //注視目標との距離
private:

	inline void updateCameraVectorsFree()
	{
		// Front vectorを計算
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Right and Up vector　を計算
		Right = glm::normalize(glm::cross(Front, WorldUp));  // 正規化
		Up = glm::normalize(glm::cross(Right, Front));
	}


};

#endif