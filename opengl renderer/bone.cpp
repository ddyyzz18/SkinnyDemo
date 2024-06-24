#pragma once
#include "bone.h"
#include <iostream>


Bone::Bone(std::string&& name, const aiNodeAnim* channel)
	:
	m_Name(std::move(name)),
	m_LocalTransform(1.0f)
{
	m_NumPositions = channel->mNumPositionKeys;
	//�@translation�s����擾
	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		m_Positions.push_back(std::move(data));
	}

	m_NumRotations = channel->mNumRotationKeys;
	//�@rotation�iin quotation form�j�s����擾
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
		data.timeStamp = timeStamp;
		m_Rotations.push_back(std::move(data));
	}

	m_NumScalings = channel->mNumScalingKeys;
	//�@scaling�s����擾
	for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.scale = AssimpGLMHelpers::GetGLMVec(scale);
		data.timeStamp = timeStamp;
		m_Scales.push_back(std::move(data));
	}
	m_Unmasked = true;

}

Bone::~Bone()
{

}

int Bone::GetPositionIndex(float animationTime, unsigned int fps)
{

	return static_cast<int>(animationTime / (1000.0f / static_cast<float>(fps)));
}


int Bone::GetRotationIndex(float animationTime, unsigned int fps)
{

	return static_cast<int>(animationTime / (1000.0f / static_cast<float>(fps)));
}

int Bone::GetScaleIndex(float animationTime, unsigned int fps)
{
	return static_cast<int>(animationTime / (1000.0f / static_cast<float>(fps)));

}

// ��ԗp�W�����v�Z
inline float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

//�@�g�����X���[�V��������
glm::mat4 Bone::InterpolatePosition(float animationTime, unsigned int fps)
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
		, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::vec3 Bone::InterpolatePositionVec3(float animationTime, unsigned int fps)
{
	if (1 == m_NumPositions)
		return glm::vec3(m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
		, scaleFactor);
	return finalPosition;
}

//�@���[�e�C�V��������
glm::mat4 Bone::InterpolateRotation(float animationTime, unsigned int fps)
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
		m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
		, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);

}

glm::quat Bone::InterpolateRotationQuat(float animationTime, unsigned int fps)
{
	if (1 == m_NumRotations)
	{
		glm::quat rotation = glm::normalize(m_Rotations[0].orientation);
		return rotation;
	}

	int p0Index = GetRotationIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
		m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
		, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return finalRotation;

}

//�@�X�P�[�����O����
glm::mat4 Bone::InterpolateScaling(float animationTime, unsigned int fps)
{
	if (1 == m_NumScalings)
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

	int p0Index = GetScaleIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
		m_Scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
		, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

glm::vec3 Bone::InterpolateScalingVec3(float animationTime, unsigned int fps)
{
	if (1 == m_NumScalings)
		return m_Scales[0].scale;

	int p0Index = GetScaleIndex(animationTime,fps);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
		m_Scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
		, scaleFactor);
	return finalScale;
}

//������:���A�j���C�V��������
//������:�u�����h�C���A�j���C�V��������
//��O����:�u�����h�C���A�j���C�V���������Ă�Bone�\����
//��l����:��ԗp�W��
void Bone::UpdateBlending(float ct, float nt, Bone* bonenext, float blendFactor, unsigned int fps)
{
	//���݃A�j���C�V�����̕ϊ�
	const glm::vec3 translation1 = InterpolatePositionVec3(ct, fps);
	const glm::quat rotation1 = InterpolateRotationQuat(ct, fps);
	const glm::vec3 scale1 = InterpolateScalingVec3(ct, fps);
	//�u�����h�C���A�j���C�V�����̕ϊ�
	const glm::vec3 translation2 = bonenext->InterpolatePositionVec3(nt, fps);
	const glm::quat rotation2 = bonenext->InterpolateRotationQuat(nt, fps);
	const glm::vec3 scale2 = bonenext->InterpolateScalingVec3(nt, fps);

	glm::vec3 blendedTrans = glm::mix(translation1, translation2, blendFactor);
	glm::quat blendedRot = glm::slerp(rotation1, rotation2, blendFactor);
	glm::vec3 blendedScl = glm::mix(scale1, scale1, blendFactor);

	/*m_LocalTransform = (glm::translate(glm::mat4(1.0f), translation1) * glm::toMat4(rotation1) * glm::scale(glm::mat4(1.0f), scale1));
	bonenext->m_LocalTransform = (glm::translate(glm::mat4(1.0f), translation2) * glm::toMat4(rotation2) * glm::scale(glm::mat4(1.0f), scale2));*/
	m_LocalTransform = (glm::translate(glm::mat4(1.0f),blendedTrans) * glm::toMat4(blendedRot) * glm::scale(glm::mat4(1.0f), blendedScl));
}

//������:���A�j���C�V��������
//������:�u�����h�C���A�j���C�V��������
//��O����:�u�����h�C���A�j���C�V���������Ă�Bone�\����
//��l����:��ԗp�W��
//��܈���:�u�����h�C���A�j�}�V�I���Ɏ{���g�����X���[�V����
//��Z����:�u�����h�C���A�j�}�V�I���Ɏ{�����[�e�C�V����
//�掵����:�u�����h�C���A�j�}�V�I���Ɏ{���X�P�C��

void Bone::UpdateBlendingRot(float ct, float nt, Bone* bonenext, float blendFactor, glm::vec3 rot, unsigned int fps)
{
	//���݃A�j���C�V�����̕ϊ�
	const glm::vec3 translation1 = InterpolatePositionVec3(ct, fps);
	const glm::quat rotation1 = InterpolateRotationQuat(ct, fps);
	const glm::vec3 scale1 = InterpolateScalingVec3(ct, fps);
	//�u�����h�C���A�j���C�V�����̕ϊ�
	glm::vec3 translation2 = bonenext->InterpolatePositionVec3(nt, fps);
	glm::quat rotation2 = bonenext->InterpolateRotationQuat(nt, fps);
	//rotation2 = glm::rotate(rotation2, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation2 = glm::rotate(rotation2, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//rotation2 = glm::rotate(rotation2, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 scale2 = bonenext->InterpolateScalingVec3(nt, fps);

	glm::vec3 blendedTrans = glm::mix(translation1, translation2, blendFactor);
	glm::quat blendedRot = glm::slerp(rotation1, rotation2, blendFactor);
	glm::vec3 blendedScl = glm::mix(scale1, scale1, blendFactor);

	m_LocalTransform = (glm::translate(glm::mat4(1.0f), blendedTrans) * glm::toMat4(blendedRot) * glm::scale(glm::mat4(1.0f), blendedScl));
}

//void Bone::UpdateMask()
//{
//	if (!m_Unmasked)
//	{
//
//	}
//}