#pragma once
#ifndef ANIDATA_H
#define ANIDATA_H
#include<glm/glm.hpp>

struct BoneInfo
{
	//　ボンド番号　（finalBoneMatricesベクトルのインデクス）
	int id;

	//　頂点をLocal Spaceから Bone Spaceへ変換
	glm::mat4 offset;

};

#endif

