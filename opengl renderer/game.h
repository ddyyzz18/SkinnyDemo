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

//メッシュフィールド生成係数
#define MESH_POS (glm::vec3(0.0f, 0.0f, 0.0f))
#define MESH_ROT (glm::vec3(0.0f, 0.0f, 0.0f))
#define MESH_X	(10)
#define MESH_Y	(10)
#define MESH_SIZE (2.0f)

//平行光源係数
#define DIR_DIR  (glm::vec3(0.0f, -1.0f, -1.0f))
#define DIR_AMIB (glm::vec3(0.2f, 0.2f, 0.2f))
#define DIR_DIFF (glm::vec3(0.7f, 0.7f, 0.7f))
#define DIR_SPEC (glm::vec3(0.3f, 0.3f, 0.3f))

class Game
{
public:
	Game();
	~Game();
	bool Init();					//初期化
	void Update();					//更新処理
	void Draw();					//描画処理
	void Run();						//ゲーム作動
	void Release();					//終了処理
private:
	bool InitGLFW();				//GLFW初期化
	bool InitOpenGL();				//opengl初期化（GLAD使用）
	bool InitResouce();				//リソースの初期化
	bool InitImgui();				//Imguiの初期化

	void DrawPolygons();			//ポリゴン描画
	void DrawImgui();				//Imgui描画
	void PreDraw();					//描画前処理

	void RenderShadow();			//シャドウマップ用テクスチャをレンダ
	void DrawQuad();				//2dテクスチャを全画面表示

	//　CALLBACK関数
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static float getAspectRatio() { return (ASPECT_RATIO); }
	static void renderQuad();					//全画面テクスチャにレンダ
	void processInput(GLFWwindow* window);		//入力処理
	void calculateFPS(GLFWwindow* window);		//FPS計算
	void ResetLight();							//光初期化
	void ReadNodeHierarchyImGui(NodeData&);		//Imgui用骨構造を表示
	void SeteUseFalse(NodeData&);				//骨非使用更新
	void SeteUseTrue(NodeData&);				//骨使用更新
	void UseAllBones();							//全骨使用

private:
	static Camera m_Camera;			//カメラ
	static Timer m_Timer;			//タイマー
	string m_WinName;				//window名

	vector<Shader> m_Shaders;		//シャディ
	vector<Model> m_Model;			//モデル
	vector<Animation> m_Animation;	//アニマシオン
	vector<Blender>	m_Blender;		//ブレンダー　
	
	vector<Player> m_Player;		//プレイヤー
	vector<Light*>  m_Light;		//ライトポイント
	vector<MeshField> m_Field;		//メッシュフィールド
	vector<ShadowMap> m_Shadow;		//シャドウマップ　

	//FPS計算用変数
	float lastTime = 0.0;
	int frameCount = 0;
	float fps = 0.0f;

	uint m_ScreenWid;				//windowの高さ
	uint m_ScreenHei;				//windowの幅
	GLFWwindow* m_Window;			//GLFWWindow　ポインタ

	static bool m_Input;			//指令入力可？
	bool m_AniPause;				//アニメイシヨン
	static bool m_CamRotEnable;		//カメラ旋回可？
	static unsigned int quadVAO;	//全画面用VAO
	static unsigned int quadVBO;	//全画面用VBO
};

#endif