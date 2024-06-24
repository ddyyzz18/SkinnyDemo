#pragma once
#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "light.h"
#include "gameobject.h"

#define DEFAULT_SM_RES (1024)
#define SM_NEAR_PLANE (1.0f)		//���C�g�t���X�^��nearplane
#define SM_FAR_PLANE (30.0f)			//���C�g�t���X�^��farplane

class ShadowMap
{
public:
	
public:
	ShadowMap(Light*, int);
	void StartShadowPass(Shader&);						//�����_to�[�x�}�b�v
	void EndShadowPass();								//�����_to�[�x�}�b�v
	void SetupLightFrustumDir(GameObject*);				//Light���_��Proj�AView�s���ݒu
	inline unsigned int GetShadowTex() { return m_DepthMap;}
	void BindShadow(Shader&);									//�`�悷��O�ɃV���h�E�}�b�v�֌WUniform�A�e�N�X�`�����o�C���h
private:
	unsigned int depthMapFBO;							//�[�x�}�b�vFBO
	const unsigned int m_ShadowMapHeight;
	const unsigned int m_ShadowMapWidth;
	glm::mat4 m_LightPV;
	Light* m_light;
	int m_LightType;									//0=Dirlight 1=Pointlight 2=SpotLight
	unsigned int m_DepthMap;							//�[�x�e�N�X�`��
private:
	void SetupShadowMap();								//FBO�Ȃǂ�ݒu

};
#endif SHADOWMAP_H