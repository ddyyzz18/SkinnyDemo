#include "animator.h"
#include <glm/gtx/euler_angles.hpp>
#define TRIVAIL_TIME (0.01f)				//ArrayIndexOutOfBounds��h������
#define MAXBONE (100)						//�ő卜��
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
	bool aniEnd = false; //�A�j�}�V�I���Đ��ς݁H
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


		// RootBone���珈�����鎞�A�������ɗ\�ߕۑ����ꂽRootBone�̕ϊ��s��̋t�s�������
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

//�������@node�F����������{���h�iNode�j 
//�������@parentTransform:��{���h�iNode�j�̕ϊ��s��
void Animator::CalculateBoneTransform(const NodeData* node, glm::mat4& parentTransform)
{
	std::string nodeName = node->name;				//���݃A�j���C�V�����̃m�[�h��
	glm::mat4 nodeTransform = node->transformation;	//�{���h�Ȃ��m�[�h�̏ꍇ

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		if (!node->unmasked)
			Bone->m_Unmasked = false;
		else
			Bone->m_Unmasked = true;
		Bone->Update(m_CurrentTime, m_CurrentAnimation->m_FPS);
		nodeTransform = Bone->GetLocalTransform();	//��{���h��Ԃւ̕ϊ��s��
	}

	//�@RootBone���������鎞�AparentTransform��RootNode��nodeTransform�̋t�s������Ă邩��AglobalTransformation���P�ʍs��ɂȂ�
	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	//�@�q�m�[�h�̏��������s
	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}


//�������@node:���������Ă���m�[�g�i�{���h�j
//������	parentTransform::��m�[�h��Ԃւ̕ϊ��s��
void Animator::CalculateBoneTransformBlend(const NodeData* node, glm::mat4& parentTransform)
{
	std::string nodeName = node->name;					//���݂��Ă�m�[�h�����擾
	glm::mat4 NodeTransform = node->transformation;		//���A�j���C�V�����̕ϊ��s��
	Bone* BoneC = m_CurrentAnimation->FindBone(nodeName);
	Bone* BoneN = m_Blender->AnimationIn->FindBone(nodeName);

	//�@�m�[�h���{���h�����Ă�ꍇ
	if (BoneC && BoneN)
	{

		BoneC->UpdateBlending(m_CurrentTime, m_BlendInTime, BoneN, m_BlenderFactor, m_CurrentAnimation->m_FPS);
		NodeTransform = BoneC->GetLocalTransform();
	}

	//RootBone���������鎞�AparentTransform��RootNode��currentTransform�̋t�s������Ă邩��AglobalTransformation���P�ʍs��ɂȂ�
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

//�������@node:���������Ă���m�[�g�i�{���h�j
//������	parentTransform::��m�[�h��Ԃւ̕ϊ��s��
//��O�����@���A�j�}�V�I���Ɏ{�����[�e�[�V����
void Animator::CalculateBoneTransformBlendRot(const NodeData* node, glm::mat4& parentTransform, glm::vec3 rot)
{
	glm::mat4 rotation_matrix = glm::eulerAngleXYZ(glm::radians(rot.x),
		glm::radians(rot.y),
		glm::radians(rot.z));

	std::string nodeName = node->name;										//���݂��Ă�m�[�h�����擾
	glm::mat4 NodeTransform = rotation_matrix * node->transformation;		//���A�j���C�V�����̕ϊ��s��
	Bone* BoneC = m_CurrentAnimation->FindBone(nodeName);
	Bone* BoneN = m_Blender->AnimationIn->FindBone(nodeName);

	//�@�m�[�h���{���h�����Ă�ꍇ
	if (BoneC && BoneN)
	{

		BoneC->Update(m_CurrentTime, m_CurrentAnimation->m_FPS);
		BoneN->Update(m_BlendInTime, m_CurrentAnimation->m_FPS);
		glm::mat4 NodeTransformc = rotation_matrix * BoneC->GetLocalTransform();
		glm::mat4 NodeTransformn = BoneN->GetLocalTransform();
		//NodeTransform = glm::mix(NodeTransformc, NodeTransformn, m_BlenderFactor);
	}

	//RootBone���������鎞�AparentTransform��RootNode��currentTransform�̋t�s������Ă邩��AglobalTransformation���P�ʍs��ɂȂ�
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