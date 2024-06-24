#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

//�f�t�H���g�u�����h�W��
#define BLD_FADEIN (0.4f)
#define BLD_START  (0.0f)

//�A�j�}�V�I���̐؂�ւ�����ۑ�
struct Blender
{
	Animation* AnimationIn;								//���ꂩ��Đ�����A�j�}�V�I��
	float FadeInTime;									//�A�j�}�V�I���؂�ւ��ς݂܂ł̎��ԁi�b�j
	float StartTime;									//�A�j�}�V�I���̉��b����Đ�
	bool Loop;											//�u�����h�C���A�j�}�V�I�������[�v�Đ��H
	bool StaticFadein;									//�u�����h�����A�j�}�V�I�����X�V���邩�H
	bool ZeroFadein;									//�u�����h���鎞���A�j�}�V�I�����[�����_�ɖ߂�H
};

class Animator
{
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;			//���_�Ɏ{���ŏI�ω��s��
	Animation* m_CurrentAnimation;						//�Đ����Ă�A�j�}�V�I��

	float m_CurrentTime;								//���ݎ��_
	float m_DeltaTime;									//�t���C���^�C��

	Blender* m_Blender;									//�g�p���̃u�����h�\����
	float m_BlendInTime;								//�u�����h�����̃A�j�}�V�I���̌��ݎ��_
	float m_BlenderFactor;								//�u�����h�W��
	bool  m_Blending;									//�A�j���C�V�����̃u�����h�C�����H
	bool  m_Loop;										//�A�j�}�V�I�������[�v�Đ��H�itrue�Fcall UpdateAnimation; false: ���݃A�j���C�V������񂾂��Đ��j
public:
	Animator(Animation* animation);
	Animator() = default;
	~Animator();

	void Showframe(int index);							//�t���C���擾
	void LoadAnimation(Animation* pAnimation);			//�A�j�}�V�I�����Đ�

	bool UpdateAnimation(float dt);						//�A�j�}�V�I���̍X�V(���[�v�o�[�W����)
	void UpdateAnimationBlend(float dt);				//�A�j�}�V�I���u�����h�̍X�V
	void UpdateAnimationBlendRot(float dt,glm::vec3 rot);
														//FinalBoneMatrices���擾
	void CalculateBoneTransform(const NodeData* node, glm::mat4& parentTransform);
	void CalculateBoneTransformBlend(const NodeData* node, glm::mat4& parentTransform);
	void CalculateBoneTransformBlendRot(const NodeData* node, glm::mat4& parentTransform, glm::vec3 rot);
														//����t���C����FinalBoneMatrices���擾
	void FetchFrameTransform(const NodeData* node, glm::mat4& parentTransform,int index);
	bool StartBlend(Blender* blender);
	void PlayAni(Blender* blender);
	void EndBlend();
	//void EndBlendRot(glm::vec3 rot);
	void SetCurrentTime(float dt);
	
	inline Animation* GetCurrentAni() { return m_CurrentAnimation; }
	inline float* GetCurrentTime() { return &m_CurrentTime; }
	inline bool GetBlending() {return m_Blending;}
	inline std::vector<glm::mat4>& GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
	unsigned int GetFrameCount()
	{
		return m_CurrentAnimation->GetTotalFrames();
	}
};

#endif
