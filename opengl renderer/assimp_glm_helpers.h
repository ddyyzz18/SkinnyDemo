#pragma once
#ifndef GLM_HELPER_H
#define GLM_HELPER_H
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>


class AssimpGLMHelpers
{
public:

	//Å@AssimppÅ@matrixÇ©ÇÁglm::mat4Å@Ç÷ïœä∑
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}

	static inline bool areMatricesEqual(const glm::mat4& mat1, const glm::mat4& mat2, float epsilon = 0.001f) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				float a = std::abs(mat1[i][j] - mat2[i][j]);
				if (a > epsilon) {
					return false;
				}
			}
		}
		return true;
	}
};

#endif