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
#define SM_NEAR_PLANE (1.0f)		//ライトフラスタムnearplane
#define SM_FAR_PLANE (30.0f)			//ライトフラスタムfarplane

class ShadowMap
{
public:
	
public:
	ShadowMap(Light*, int);
	void StartShadowPass(Shader&);						//レンダto深度マップ
	void EndShadowPass();								//レンダto深度マップ
	void SetupLightFrustumDir(GameObject*);				//Light視点のProj、View行列を設置
	inline unsigned int GetShadowTex() { return m_DepthMap;}
	void BindShadow(Shader&);									//描画する前にシャドウマップ関係Uniform、テクスチャをバインド
private:
	unsigned int depthMapFBO;							//深度マップFBO
	const unsigned int m_ShadowMapHeight;
	const unsigned int m_ShadowMapWidth;
	glm::mat4 m_LightPV;
	Light* m_light;
	int m_LightType;									//0=Dirlight 1=Pointlight 2=SpotLight
	unsigned int m_DepthMap;							//深度テクスチャ
private:
	void SetupShadowMap();								//FBOなどを設置

};
#endif SHADOWMAP_H