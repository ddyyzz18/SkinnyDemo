#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "bone.h"
#include <functional>
#include "animdata.h"
#include "model_animation.h"


class Animation
{
private:
	float m_Duration;					//アニメーションの長さ
	int m_TicksPerSecond;				//秒間フレイム数
	std::vector<Bone> m_Bones;
	Model* m_Model;
	glm::mat4 m_RootInvTransformation;	//RootNodeのtransformの逆行列を保存
	unsigned int m_TotalFrames;			//アニメイシヨンのフレイム数
public:
	unsigned int m_FPS;					//秒間フレイム数
public:
	Animation(const std::string& animationPath, Model* model);
	~Animation();
	Bone* FindBone(const std::string& name);
	inline int& GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float& GetDuration() { return m_Duration; }
	inline glm::mat4& GetRootInvTrans() { return m_RootInvTransformation; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_Model->GetBoneInfoMap();
	}
	inline const unsigned int GetTotalFrames() { return m_TotalFrames; };
	inline Model* GetModel() { return m_Model; }
private:
	
	void LoadBones(const aiAnimation* animation, Model& model);

};

#endif
