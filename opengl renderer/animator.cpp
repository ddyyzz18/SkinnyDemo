#include "animator.h"
#include <glm/gtx/euler_angles.hpp>
#define TRIVAIL_TIME (0.01f)				//ArrayIndexOutOfBoundsを防ぐため
#define MAXBONE (100)						//最大骨数
Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	m_DeltaTime = 0.0f;
	m_BlendInTime = 0.0f;
	m_FinalBoneMatrices.reserve(MAXBONE);
	m_BlenderFactor = 0.0f;
	m_Blending = false;
	m_Blender = nullptr;
	m_Loop = true;
	for (int i = 0; i < MAXBONE; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

Animator::~Animator()
{

}

void Animator::LoadAnimation(Animation* pAnimation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = pAnimation;
	m_DeltaTime = 0.0f;
	m_BlendInTime = 0.0f;
	m_FinalBoneMatrices.reserve(MAXBONE);
	m_BlenderFactor = 0.0f;
	m_Blending = false;
	m_Blender = nullptr;
	m_Loop = true;
	for (int i = 0; i < MAXBONE; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

bool Animator::StartBlend(Blender* blender)
{
	if ((!m_Blending) && (blender->AnimationIn != m_CurrentAnimation))
	{
		m_Blender = blender;
		m_BlendInTime = blender->StartTime;
		m_BlenderFactor = 0.0f;
		m_Blending = true;
		m_Loop = blender->Loop;
		return true;
	}
	return false;

}

void Animator::EndBlend()
{
	m_CurrentTime = m_BlendInTime;
	m_BlendInTime = 0.0f;
	m_BlenderFactor = 0.0f;
	m_Blending = false;
	m_CurrentAnimation = m_Blender->AnimationIn;
	m_Blender = nullptr;
}

void Animator::PlayAni(Blender* blender)
{
	m_CurrentTime = 0.0f;
	m_BlendInTime = 0.0f;
	m_BlenderFactor = 0.0f;
	m_Blending = false;
	m_CurrentAnimation = blender->AnimationIn;
	m_Blender = nullptr;
}

void Animator::UpdateAnimationBlend(float dt)
{
	m_DeltaTime = dt;

	if (m_CurrentAnimation)
	{
		if (m_Blender->ZeroFadein)
		{
			m_CurrentTime = 0.0f;
		}
		else if (m_Blender->StaticFadein)
		{

		}
		else
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		}

		m_BlendInTime += m_Blender->AnimationIn->GetTicksPerSecond() * dt;
		m_BlenderFactor += (dt* m_CurrentAnimation->GetTicksPerSecond()) / (m_Blender->FadeInTime);
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		m_BlendInTime = fmod(m_BlendInTime, m_Blender->AnimationIn->GetDuration());
		if (m_BlenderFactor >= 1.0f)
		{
			EndBlend();
			CalculateBoneTransform(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans());
		}
		else
		{
			CalculateBoneTransformBlend(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans());
		}


	}
}

void Animator::UpdateAnimationBlendRot(float dt, glm::vec3 rot)
{
	m_DeltaTime = dt;

	if (m_CurrentAnimation)
	{
		if ((!m_Blender->StaticFadein))
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		}
		m_BlendInTime += m_Blender->AnimationIn->GetTicksPerSecond() * dt;
		m_BlenderFactor += (dt) / (m_Blender->FadeInTime);
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		m_BlendInTime = fmod(m_BlendInTime, m_Blender->AnimationIn->GetDuration());
		if (m_BlenderFactor >= 1.0f)
		{
			EndBlend();
			CalculateBoneTransform(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans());
		}
		else
		{
			CalculateBoneTransformBlendRot(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans(), rot);
		}


	}
}

bool Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	bool aniEnd = false; //アニマシオン再生済み？
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		if (m_Loop)
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		else if (m_CurrentTime >= m_CurrentAnimation->GetDuration())
		{
			m_CurrentTime = m_CurrentAnimation->GetDuration() - TRIVAIL_TIME;
			aniEnd = true;
		}


		// RootBoneから処理する時、第二引数に予め保存されたRootBoneの変換行列の逆行列を入れる
		CalculateBoneTransform(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans());
	}

	return aniEnd;
}


void Animator::Showframe(int index)
{
	if (m_CurrentAnimation)
	{
		FetchFrameTransform(&m_CurrentAnimation->GetModel()->GetRootNode(), m_CurrentAnimation->GetRootInvTrans(), index);
	}
}

//第一引数　node：今処理するボンド（Node） 
//第二引数　parentTransform:母ボンド（Node）の変換行列
void Animator::CalculateBoneTransform(const NodeData* node, glm::mat4& parentTransform)
{
	std::string nodeName = node->name;				//現在アニメイシヨンのノード名
	glm::mat4 nodeTransform = node->transformation;	//ボンドなしノードの場合

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		if (!node->unmasked)
			Bone->m_Unmasked = false;
		else
			Bone->m_Unmasked = true;
		Bone->Update(m_CurrentTime, m_CurrentAnimation->m_FPS);
		nodeTransform = Bone->GetLocalTransform();	//母ボンド空間への変換行列
	}

	//　RootBoneを処理する時、parentTransformにRootNodeのnodeTransformの逆行列を入れてるから、globalTransformationが単位行列になる
	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	//　子ノードの処理を実行
	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}


//第一引数　node:今処理しているノート（ボンド）
//第二引数	parentTransform::母ノード空間への変換行列
void Animator::CalculateBoneTransformBlend(const NodeData* node, glm::mat4& parentTransform)
{
	std::string nodeName = node->name;					//現在してるノード名を取得
	glm::mat4 NodeTransform = node->transformation;		//現アニメイシヨンの変換行列
	Bone* BoneC = m_CurrentAnimation->FindBone(nodeName);
	Bone* BoneN = m_Blender->AnimationIn->FindBone(nodeName);

	//　ノードがボンド持ってる場合
	if (BoneC && BoneN)
	{

		BoneC->UpdateBlending(m_CurrentTime, m_BlendInTime, BoneN, m_BlenderFactor, m_CurrentAnimation->m_FPS);
		NodeTransform = BoneC->GetLocalTransform();
	}

	//RootBoneを処理する時、parentTransformにRootNodeのcurrentTransformの逆行列を入れてるから、globalTransformationが単位行列になる
	glm::mat4 globalTransformationBlended = parentTransform * NodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformationBlended * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransformBlend(&node->children[i], globalTransformationBlended);
}

//第一引数　node:今処理しているノート（ボンド）
//第二引数	parentTransform::母ノード空間への変換行列
//第三引数　現アニマシオンに施すローテーション
void Animator::CalculateBoneTransformBlendRot(const NodeData* node, glm::mat4& parentTransform, glm::vec3 rot)
{
	glm::mat4 rotation_matrix = glm::eulerAngleXYZ(glm::radians(rot.x),
		glm::radians(rot.y),
		glm::radians(rot.z));

	std::string nodeName = node->name;										//現在してるノード名を取得
	glm::mat4 NodeTransform = rotation_matrix * node->transformation;		//現アニメイシヨンの変換行列
	Bone* BoneC = m_CurrentAnimation->FindBone(nodeName);
	Bone* BoneN = m_Blender->AnimationIn->FindBone(nodeName);

	//　ノードがボンド持ってる場合
	if (BoneC && BoneN)
	{

		BoneC->Update(m_CurrentTime, m_CurrentAnimation->m_FPS);
		BoneN->Update(m_BlendInTime, m_CurrentAnimation->m_FPS);
		glm::mat4 NodeTransformc = rotation_matrix * BoneC->GetLocalTransform();
		glm::mat4 NodeTransformn = BoneN->GetLocalTransform();
		//NodeTransform = glm::mix(NodeTransformc, NodeTransformn, m_BlenderFactor);
	}

	//RootBoneを処理する時、parentTransformにRootNodeのcurrentTransformの逆行列を入れてるから、globalTransformationが単位行列になる
	glm::mat4 globalTransformationBlended = parentTransform * NodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformationBlended * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransformBlendRot(&node->children[i], globalTransformationBlended, rot);
}

void Animator::FetchFrameTransform(const NodeData* node, glm::mat4& parentTransform, int index)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{

		Bone->FetchFrame(index);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		FetchFrameTransform(&node->children[i], globalTransformation, index);
}

void Animator::SetCurrentTime(float dt)
{
	m_CurrentTime = dt;
}