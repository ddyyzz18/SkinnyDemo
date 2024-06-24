#pragma once
#ifndef BONE_H
#define BONE_H

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "assimp_glm_helpers.h"


struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;

};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{	
public:
	bool m_Unmasked;			// 骨の影響をマスクしてるか（true＝使用中　false＝マスク中）
private:
	//keyframe transform matrixを記録
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;

	//アニマシオンの総フレイム数　（３０frames／１ｓ）
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;


	glm::mat4 m_LocalTransform; // Localtransform行列を保存（母ボンドSapceへ変換）
	std::string m_Name;			// ボンド名


public:
	Bone(std::string&& name, const aiNodeAnim* channel);
	~Bone();

	// Localtransform行列の更新
	void Update(float animationTime, unsigned int fps)
	{
		if (m_Unmasked)
		{
			glm::mat4 translation = InterpolatePosition(animationTime, fps);
			glm::mat4 rotation = InterpolateRotation(animationTime, fps);
			glm::mat4 scale = InterpolateScaling(animationTime, fps);
			m_LocalTransform = translation * rotation * scale;
		}
		else
		{
			glm::mat4 translation = InterpolatePosition(0.0f, fps);
			glm::mat4 rotation = InterpolateRotation(0.0f, fps);
			glm::mat4 scale = InterpolateScaling(0.0f, fps);
			m_LocalTransform = translation * rotation * scale;
		}

	}

	//　フレイムごと再生用
	inline void FetchFrame(int index)
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Positions[index].position);
		glm::mat4 rotation = glm::toMat4(m_Rotations[index].orientation);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scales[index].scale);
		m_LocalTransform = translation * rotation * scale;
	}
	inline glm::mat4 GetLocalTransform() { return m_LocalTransform; }
	inline std::string GetBoneName() const { return m_Name; }


	void UpdateBlending(float ct, float nt, Bone* bonenext, float blendFactor, unsigned int fps);
	void UpdateBlendingRot(float ct, float nt, Bone* bonenext, float blendFactor, glm::vec3 rot, unsigned int fps);
	//　補間frame番号を取得
	int GetPositionIndex(float animationTime, unsigned int fps);
	int GetRotationIndex(float animationTime, unsigned int fps);
	int GetScaleIndex(float animationTime, unsigned int fps);
	//void UpdateMask();

private:

	//　補間
	inline float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float animationTime, unsigned int fps);
	glm::mat4 InterpolateRotation(float animationTime, unsigned int fps);
	glm::mat4 InterpolateScaling(float animationTime, unsigned int fps);

	//　補間(ベクトルバージョン)
	glm::vec3 InterpolatePositionVec3(float animationTime, unsigned int fps);
	glm::quat InterpolateRotationQuat(float animationTime, unsigned int fps);
	glm::vec3 InterpolateScalingVec3(float animationTime, unsigned int fps);



};
#endif
