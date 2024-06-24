#pragma once
#ifndef GAME_H
#define GAME_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "light.h"
#include "model_animation.h"
#include "animation.h"
#include "animator.h"
#include "timer.h"
#include "player.h"
#include "meshfield.h"
#include "shadowmap.h"

//���b�V���t�B�[���h�����W��
#define MESH_POS (glm::vec3(0.0f, 0.0f, 0.0f))
#define MESH_ROT (glm::vec3(0.0f, 0.0f, 0.0f))
#define MESH_X	(10)
#define MESH_Y	(10)
#define MESH_SIZE (2.0f)

//���s�����W��
#define DIR_DIR  (glm::vec3(0.0f, -1.0f, -1.0f))
#define DIR_AMIB (glm::vec3(0.2f, 0.2f, 0.2f))
#define DIR_DIFF (glm::vec3(0.7f, 0.7f, 0.7f))
#define DIR_SPEC (glm::vec3(0.3f, 0.3f, 0.3f))

class Game
{
public:
	Game();
	~Game();
	bool Init();					//������
	void Update();					//�X�V����
	void Draw();					//�`�揈��
	void Run();						//�Q�[���쓮
	void Release();					//�I������
private:
	bool InitGLFW();				//GLFW������
	bool InitOpenGL();				//opengl�������iGLAD�g�p�j
	bool InitResouce();				//���\�[�X�̏�����
	bool InitImgui();				//Imgui�̏�����

	void DrawPolygons();			//�|���S���`��
	void DrawImgui();				//Imgui�`��
	void PreDraw();					//�`��O����

	void RenderShadow();			//�V���h�E�}�b�v�p�e�N�X�`���������_
	void DrawQuad();				//2d�e�N�X�`����S��ʕ\��

	//�@CALLBACK�֐�
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static float getAspectRatio() { return (ASPECT_RATIO); }
	static void renderQuad();					//�S��ʃe�N�X�`���Ƀ����_
	void processInput(GLFWwindow* window);		//���͏���
	void calculateFPS(GLFWwindow* window);		//FPS�v�Z
	void ResetLight();							//��������
	void ReadNodeHierarchyImGui(NodeData&);		//Imgui�p���\����\��
	void SeteUseFalse(NodeData&);				//����g�p�X�V
	void SeteUseTrue(NodeData&);				//���g�p�X�V
	void UseAllBones();							//�S���g�p

private:
	static Camera m_Camera;			//�J����
	static Timer m_Timer;			//�^�C�}�[
	string m_WinName;				//window��

	vector<Shader> m_Shaders;		//�V���f�B
	vector<Model> m_Model;			//���f��
	vector<Animation> m_Animation;	//�A�j�}�V�I��
	vector<Blender>	m_Blender;		//�u�����_�[�@
	
	vector<Player> m_Player;		//�v���C���[
	vector<Light*>  m_Light;		//���C�g�|�C���g
	vector<MeshField> m_Field;		//���b�V���t�B�[���h
	vector<ShadowMap> m_Shadow;		//�V���h�E�}�b�v�@

	//FPS�v�Z�p�ϐ�
	float lastTime = 0.0;
	int frameCount = 0;
	float fps = 0.0f;

	uint m_ScreenWid;				//window�̍���
	uint m_ScreenHei;				//window�̕�
	GLFWwindow* m_Window;			//GLFWWindow�@�|�C���^

	static bool m_Input;			//�w�ߓ��͉H
	bool m_AniPause;				//�A�j���C�V����
	static bool m_CamRotEnable;		//�J��������H
	static unsigned int quadVAO;	//�S��ʗpVAO
	static unsigned int quadVBO;	//�S��ʗpVBO
};

#endif