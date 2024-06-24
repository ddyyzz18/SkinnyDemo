#pragma once
#ifndef G_OBJECT_H
#define G_OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class GameObject
{
public:
	glm::vec3 pos;			//位置
	glm::vec3 rot;			//ローテーション
	glm::vec3 scl;			//スケーリング
	glm::mat4 m_WorldTrans;	//ワールド行列
public:
	GameObject() = default;
	~GameObject() {};

	inline glm::mat4 GetTrans()
	{
		return glm::translate(glm::mat4(1.0f), pos);
	}

	inline glm::mat4 GetQuat()
	{
		return  glm::toMat4(glm::quat(glm::radians(rot)));
	}

	inline glm::mat4 GetScling()
	{
		return  glm::scale(glm::mat4(1.0f), scl);
	}

	inline glm::mat4 GetModel()
	{
		return (glm::translate(glm::mat4(1.0f), pos) * (glm::toMat4(glm::quat(glm::radians(rot)))) * (glm::scale(glm::mat4(1.0f), scl)));

	}

	inline glm::vec3& GetPos()
	{
		return pos;
	}

	inline glm::vec3& GetRot()
	{
		return rot;
	}

	inline glm::vec3& GetScl()
	{
		return scl;
	}

	inline void UpdateWorldTrans()
	{
		m_WorldTrans = GetModel();
	}

	inline void UpdatePRS()
	{
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(m_WorldTrans, scl, rotation, pos, skew, perspective);

		rot = glm::degrees(glm::eulerAngles(rotation));

	}
};
#endif