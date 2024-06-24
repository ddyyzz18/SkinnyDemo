#include "shadowmap.h"
#include "camera.h"

ShadowMap::ShadowMap(Light* light, int type) :m_ShadowMapHeight(DEFAULT_SM_RES), m_ShadowMapWidth(DEFAULT_SM_RES), m_LightType(type), m_light(light)
{
	SetupShadowMap();
}

void ShadowMap::SetupShadowMap()
{
	//FBO生成
	glGenFramebuffers(1, &depthMapFBO);
	//深度バッファ生成

	glGenTextures(1, &m_DepthMap);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		m_ShadowMapWidth, m_ShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//FBOにバインド
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowMap::StartShadowPass(Shader& shader)
{
	shader.use();
	shader.setMat4("lightPV", m_LightPV);
	glViewport(0, 0, m_ShadowMapWidth, m_ShadowMapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::EndShadowPass()
{
	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::SetupLightFrustumDir(GameObject* ob)
{
	if (m_LightType == 0)//DirLightの場合
	{
		DirLight* dir = static_cast<DirLight*>(m_light);
		glm::mat4 lightProjection, lightView;

		lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, SM_NEAR_PLANE, SM_FAR_PLANE);
		lightView = glm::lookAt(ob->GetPos() - dir->direction*2.0f, ob->GetPos(), glm::vec3(0.0, 1.0, 0.0));
		m_LightPV = lightProjection * lightView;
	}
}

void ShadowMap::BindShadow(Shader& shader)
{
	shader.setMat4("lightPV", m_LightPV);//lightPV行列をバインド
	glActiveTexture(GL_TEXTURE8);
	shader.setInt("shadowMap", 8);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
}