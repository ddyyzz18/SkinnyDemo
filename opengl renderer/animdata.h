#pragma once
#ifndef ANIDATA_H
#define ANIDATA_H
#include<glm/glm.hpp>

struct BoneInfo
{
	//�@�{���h�ԍ��@�ifinalBoneMatrices�x�N�g���̃C���f�N�X�j
	int id;

	//�@���_��Local Space���� Bone Space�֕ϊ�
	glm::mat4 offset;

};

#endif

