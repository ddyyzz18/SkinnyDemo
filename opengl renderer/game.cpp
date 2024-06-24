#include "game.h"
#include "keyboard.h"
#include "player.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"
#define FPS_REFRSH_INTERVAL (0.1f)
//静的メンバー変数の初期化
Camera Game::m_Camera(glm::vec3(0.0f, 0.5f, 3.0f));
Timer Game::m_Timer;
bool Game::m_Input = true;
bool Game::m_CamRotEnable = true;
unsigned int Game::quadVAO = 0;
unsigned int Game::quadVBO;

//　リソース終了処理
void Game::Release()
{
	for (int i = 0; i < m_Light.size(); i++)
	{
		if (m_Light[i])
			delete m_Light[i];
	}
	m_Light.clear();
	m_Light.shrink_to_fit();

	for (int i = 0; i < m_Shaders.size(); i++)
	{
		m_Shaders[i].ReleaseProgram();
	}
	m_Shaders.clear();
	m_Shaders.shrink_to_fit();

	for (int i = 0; i < m_Model.size(); i++)
	{
		m_Model[i].Uninit();
	}
	m_Model.clear();
	m_Model.shrink_to_fit();

	m_Animation.clear();
	m_Animation.shrink_to_fit();

	m_Blender.clear();
	m_Blender.shrink_to_fit();

	for (int i = 0; i < m_Field.size(); i++)
	{
		m_Field[i].Clear();
	}
	m_Field.clear();
	m_Field.shrink_to_fit();

	m_Player.clear();
	m_Player.shrink_to_fit();
}

//コンストラクタ
Game::Game() :
	m_ScreenWid(SCR_WIDTH), m_ScreenHei(SCR_HEIGHT), m_WinName("My Game"), m_AniPause(true)
{

}

//デストロイヤー
Game::~Game()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	Release();
	glfwTerminate();
}

//初期化
bool Game::Init()
{
	if (!InitGLFW())
		return false;
	if (!InitOpenGL())
		return false;
	if (!InitImgui())
		return false;
	if (!InitResouce())
		return false;

	return true;
}

//更新処理
void Game::Update()
{
	//Timerを初期化
	static bool first = true;
	if (first)
	{
		m_Timer.Reset();
		first = false;
	}
	KeyBoard::UpdateKeyboard(m_Window);
	if (m_Input)
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	processInput(m_Window);
	//カメラ移動禁止の時、マウスを表示

	//タイマーの更新
	m_Timer.Tick();

	//アニマシオンの更新
	if (m_AniPause)
	{
		for (int i = 0; i < m_Player.size(); i++)
			m_Player[i].Update(m_Timer.DeltaTime());
	}
	else
	{
		for (int i = 0; i < m_Player.size(); i++)
			m_Player[i].Update(0.0f);
	}
}

//　描画処理
void Game::Draw()
{
	PreDraw();
	RenderShadow();
	DrawPolygons();

	DrawImgui();

	glfwSwapBuffers(m_Window);
	glfwPollEvents();

}

void Game::PreDraw()
{
	//バッファのクリア設置
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();
	m_Player[0].UpdatePlayerVectors(m_Camera);
}

void Game::DrawPolygons()
{
	Shader& ourShader = m_Shaders[0];
	ourShader.use();

	m_Shadow[0].BindShadow(ourShader);
	m_Camera.BindCamera(ourShader);
	for (int i = 0; i < m_Player.size(); i++)
	{
		m_Player[i].BindFinalBoneMatrices(ourShader);
		m_Player[i].BindWorldMatrix(ourShader);
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_Light[0]->BindLight(m_Shaders[0]);
	for (int i = 0; i < m_Player.size(); i++)
		m_Player[i].Draw(m_Shaders[0]);

	m_Shaders[1].use();
	m_Shadow[0].BindShadow(m_Shaders[1]);
	glm::mat3 model = glm::mat3(1.0f);
	m_Camera.BindCamera(m_Shaders[1]);
	m_Shaders[1].setMat3("model", model);
	reinterpret_cast<DirLight*>(m_Light[0])->BindLight(m_Shaders[1]);
	for (int i = 0; i < m_Field.size(); i++)
		m_Field[i].DrawMeshField(m_Shaders[1]);
}

void Game::RenderShadow()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glCullFace(GL_FRONT);
	Shader& ourShader = m_Shaders[2];
	m_Shadow[0].SetupLightFrustumDir(&m_Player[0]);
	m_Shadow[0].StartShadowPass(ourShader);
	for (int i = 0; i < m_Player.size(); i++)
	{
		m_Player[i].BindFinalBoneMatrices(ourShader);
		m_Player[i].BindWorldMatrix(ourShader);
		m_Player[i].Draw(ourShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_Shadow[0].EndShadowPass();
	//glCullFace(GL_BACK);
}

void Game::DrawImgui()
{
	//プレイヤー情報
	ImGui::Begin("Player");
	if (ImGui::CollapsingHeader("Position"))
	{
		ImGui::SliderFloat("x", &m_Player[0].pos.x, -50.0f, 50.0f);
		ImGui::SliderFloat("y", &m_Player[0].pos.y, 0.0f, 50.0f);
		ImGui::SliderFloat("z", &m_Player[0].pos.z, -50.0f, 50.0f);

	}
	if (ImGui::CollapsingHeader("Rotation"))
	{
		ImGui::SliderFloat("pitch", &m_Player[0].rot.x, -360.0f, 360.0f);
		ImGui::SliderFloat("yaw", &m_Player[0].rot.y, -360.0f, 360.0f);
		ImGui::SliderFloat("roll", &m_Player[0].rot.z, -360.0f, 360.0f);

	}
	if (ImGui::Button("Reset")) {
		m_Player[0].Reset();
	}


	ImGui::End();

	ImGui::Begin("Light");
	ImGui::Text("Ambient");
	ImGui::SliderFloat("R", &m_Light[0]->ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat("G", &m_Light[0]->ambient.y, 0.0f, 1.0f);
	ImGui::SliderFloat("B", &m_Light[0]->ambient.z, 0.0f, 1.0f);

	ImGui::Text("Diffuse");
	ImGui::SliderFloat("R##foo1", &m_Light[0]->diffuse.x, 0.0f, 1.0f);
	ImGui::SliderFloat("G##foo1", &m_Light[0]->diffuse.y, 0.0f, 1.0f);
	ImGui::SliderFloat("B##foo1", &m_Light[0]->diffuse.z, 0.0f, 1.0f);

	ImGui::Text("Specular");
	ImGui::SliderFloat("R##foo2", &m_Light[0]->specular.x, 0.0f, 1.0f);
	ImGui::SliderFloat("G##foo2", &m_Light[0]->specular.y, 0.0f, 1.0f);
	ImGui::SliderFloat("B##foo2", &m_Light[0]->specular.z, 0.0f, 1.0f);
	if (ImGui::Button("Reset")) {
		ResetLight();
	}

	ImGui::End();
	ImGui::Begin("Aniamtion");
	ImGui::Checkbox("Play Animation", &m_AniPause);
	const char* items[] = { "Idling", "Idling2", "Walking", "Running", "Walking back", "Running back", "Uturn", "Left turn", "Right turn", "Attack", "Block", "Rolling", "Attack2" };
	static int item_current_idx = 0;
	static int item_blend_idx = 0;
	static Blender blender;
	static bool blendInit = false;
	if (!blendInit)
	{
		blender.AnimationIn = m_Player[0].RetrieveAni(item_blend_idx);
		blender.FadeInTime = 0.0f;
		blender.StartTime = 0.0f;
		blender.Loop = true;
		blender.StaticFadein = false;
		blender.ZeroFadein = false;
		blendInit = true;
	}


	//if (!m_Input)
	{
		item_current_idx = m_Player[0].GetAniIndex();
		if (item_current_idx > 12)
			item_current_idx = 0;
	}

	const char* combo_preview_value = items[item_current_idx];
	const char* combo_preview_value2 = items[item_blend_idx];
	if (ImGui::BeginCombo("Current Animation", combo_preview_value))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{

			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_current_idx = n;
				m_Player[0].PlayAni(static_cast<PlayerMove>(item_current_idx));
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SliderFloat("Time:ms", m_Player[0].m_Animator.GetCurrentTime(), 0.0f, m_Player[0].m_Animator.GetCurrentAni()->GetDuration());

	if (ImGui::BeginCombo("Animation 2", combo_preview_value2))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{

			const bool is_selected = (item_blend_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_blend_idx = n;
				blender.AnimationIn = m_Player[0].RetrieveAni(item_blend_idx);
				if (blender.StartTime > blender.AnimationIn->GetDuration())
					blender.StartTime = blender.AnimationIn->GetDuration();
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SliderFloat("Start Time:ms", &blender.StartTime, 0.0f, blender.AnimationIn->GetDuration());
	ImGui::SliderFloat("Fade In Time:ms", &blender.FadeInTime, 0.0f, 450.00f);
	if (ImGui::Button("Blend")) {
		m_Player[0].StartBlend(&blender, item_blend_idx);
	}
	ImGui::End();
	ImGui::Begin("Bone Hierarchy");
	if (ImGui::CollapsingHeader("Bone"))
	{
		if (ImGui::Button("Use all bones")) {
			UseAllBones();
		}
		ReadNodeHierarchyImGui(m_Player[0].m_Animator.GetCurrentAni()->GetModel()->GetRootNode());

	}
	ImGui::End();



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

//　ゲームを実行
void Game::Run()
{

	if (Init())
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			Update();
			Draw();
#ifdef _DEBUG
			calculateFPS(m_Window);
#endif
		}
	}
	else
	{
		Release();
	}
}

//GLFWの初期化
bool Game::InitGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_Window = glfwCreateWindow(m_ScreenWid, m_ScreenHei, m_WinName.c_str(), NULL, NULL);
	if (m_Window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_Window);
	//CALLBACK関数の設定（静的メンバー関数を使用）
	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_Window, mouse_callback);
	glfwSetScrollCallback(m_Window, scroll_callback);
	glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
	//mouseを非表示
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

bool Game::InitOpenGL()
{
	// glad: load all OpenGL function pointers
	// gladを使用　OpenGL関連関数のポインタを取得

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	//深度テスト
	glEnable(GL_DEPTH_TEST);
	//シェーダ
	m_Shaders.reserve(3);
	m_Shaders.emplace_back("anime_normal.vs", "anime_normal.fs");
	m_Shaders.emplace_back("field_normal.vs", "field_normal.fs");
	m_Shaders.emplace_back("shadowmap.vs", "shadowmap.fs");
	m_Shaders.emplace_back("debug_quad.vs", "debug_quad_depth.fs");
	glEnable(GL_CULL_FACE);

	return true;
}

bool Game::InitImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // キーボード操作許可
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // ゲームパッド操作許可
	ImGui::StyleColorsDark();
	//GLFW、OPEN対応初期化
	return (ImGui_ImplGlfw_InitForOpenGL(m_Window, true) && ImGui_ImplOpenGL3_Init("#version 330"));
}

//　資源の初期化
bool Game::InitResouce()
{
	//モデルを読み込む
	m_Model.emplace_back("../MODEL/Maria/Maria.dae");
	//Normalmapを読み込む
	Texture normal = Model::loadMaterialTexturesManully("textures/maria_normal.png", std::string("texture_normal"), "../MODEL/Maria");
	m_Model[0].m_Meshes[0].m_Textures.emplace_back(std::move(normal));

	//アニメーションを読み込む
	m_Animation.reserve(12);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Idle.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Warrior Idle.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Walk.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Run.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Walk Back.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Run Back.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword 180 Turn.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Turn Left.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Turn Right.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Slash Single.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Blocking.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Standing Dive Forward.dae", &m_Model[0]);
	m_Animation.emplace_back("../MODEL/Maria/Great Sword Jump Attack.dae", &m_Model[0]);

	Player player;
	for (int i = 0; i < m_Model.size(); i++)
		player.LoadModel(&m_Model[i]);
	for (int i = 0; i < m_Animation.size(); i++)
		player.LoadAnimation(&m_Animation[i]);
	player.SetupAnimator();

	//アニメーションブレンド用ブレンダーオブジェ
	m_Blender.reserve(14);
	for (int i = 0; i < m_Animation.size(); i++)
	{
		Blender blender;
		blender.AnimationIn = &m_Animation[i];
		blender.FadeInTime = BLD_FADEIN * blender.AnimationIn->GetTicksPerSecond();
		blender.StartTime = BLD_START * blender.AnimationIn->GetTicksPerSecond();
		blender.Loop = true;
		blender.StaticFadein = false;
		blender.ZeroFadein = false;
		m_Blender.emplace_back(std::move(blender));

	}
	m_Blender[U_TURN].Loop = false;
	m_Blender[ATTACK1].Loop = false;
	m_Blender[BLOCK].Loop = false;
	m_Blender[DODGE].Loop = false;

	Blender blender;
	blender.AnimationIn = &m_Animation[IDLE];
	blender.FadeInTime = BLD_FADEIN * blender.AnimationIn->GetTicksPerSecond();
	blender.StartTime = BLD_START * blender.AnimationIn->GetTicksPerSecond();
	blender.Loop = true;
	blender.StaticFadein = false;
	blender.ZeroFadein = true;
	m_Blender.emplace_back(std::move(blender));
	blender.StaticFadein = true;
	blender.ZeroFadein = false;
	m_Blender.emplace_back(std::move(blender));

	for (int i = 0; i < m_Blender.size(); i++)
		player.LoadBlender(&m_Blender[i]);

	//プレイヤー生成
	m_Player.push_back(std::move(player));

	//カメラをプレイヤーに注目（ThirdPeron　Camera）
	m_Camera.LoadFoucs(&m_Player[0].GetPos());

	//Lightの設置
	DirLight* light = new DirLight();
	light->direction = DIR_DIR;
	light->ambient = DIR_AMIB;
	light->diffuse = DIR_DIFF;
	light->specular = DIR_SPEC;
	light->use = true;
	Shader& ourShader = m_Shaders[0];
	ourShader.use();
	light->BindLight(ourShader);
	m_Light.emplace_back(std::move(light));

	//シャドウマップ
	m_Shadow.emplace_back(light, 0);		//0=Dirlight

	//メッシュフィールド
	m_Field.emplace_back(MESH_POS, MESH_ROT, MESH_X, MESH_Y, MESH_SIZE, MESH_SIZE);
	m_Field[0].LoadTexture(std::string("../TEXTURE/wall.jpg"), std::string("texture_diffuse"));
	m_Field[0].LoadTexture(std::string("../TEXTURE/NormalMap.jpg"), std::string("texture_normal"));
	m_Field[0].LoadTexture(std::string("../TEXTURE/SpecularMap.jpg"), std::string("texture_specular"));
	return true;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// Windowサイト調整用Callback関数
void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	int new_width = width;
	int new_height = height;

	float window_aspect_ratio = (float)width / (float)height;
	float aspect_ratio = getAspectRatio();
	if (window_aspect_ratio > aspect_ratio)
	{

		new_width = static_cast<int>(height * aspect_ratio);
	}
	else
	{

		new_height = static_cast<int>(width / aspect_ratio);
	}

	glViewport((width - new_width) / 2, (height - new_height) / 2, new_width, new_height);


}

// glfw: whenever the mouse moves, this callback is called
// mouse操作Callback関数
void Game::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	static bool firstMouse = true;
	static float lastX, lastY;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	if (m_Input)
	{
		m_Camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else if (m_CamRotEnable)
		m_Camera.ProcessMouseMovement(xoffset, yoffset);


}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// マウス　スクロールCallback関数
void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (m_Input)
		m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Game::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{

	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{

	}
}

//キーボード入力処理
void Game::processInput(GLFWwindow* window)
{
	//退出
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	Player& player = m_Player[0];
	player.m_Unable = false;

	bool input = false;


	//カメラ操作
	if (KeyBoard::IskeyPressed(window, GLFW_KEY_UP))
		m_Camera.ProcessKeyboard(Camera_Movement::C_FORWARD, m_Timer.DeltaTime());
	if (KeyBoard::IskeyPressed(window, GLFW_KEY_DOWN))
		m_Camera.ProcessKeyboard(Camera_Movement::C_BACKWARD, m_Timer.DeltaTime());
	if (KeyBoard::IskeyPressed(window, GLFW_KEY_LEFT))
		m_Camera.ProcessKeyboard(Camera_Movement::C_LEFT, m_Timer.DeltaTime());
	if (KeyBoard::IskeyPressed(window, GLFW_KEY_RIGHT))
		m_Camera.ProcessKeyboard(Camera_Movement::C_RIGHT, m_Timer.DeltaTime());



	//各自アニマシオンを再生
	if (KeyBoard::IskeyTriggered(window, GLFW_KEY_R))
	{
		player.Reset();
	}
	if (m_Input)
	{
		//プレイヤー操作
		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_L))
			m_Player[0].ProcessKeyboard(InputState::P_LOCK, m_Timer.DeltaTime(), m_Camera);

		if (KeyBoard::IskeyPressed(window, GLFW_KEY_W))
		{
			m_Player[0].ProcessKeyboard(InputState::P_FORWARD, m_Timer.DeltaTime(), m_Camera);
			input = true;
		}
		else if (KeyBoard::IskeyPressed(window, GLFW_KEY_S))
		{
			m_Player[0].ProcessKeyboard(InputState::P_BACKWARD, m_Timer.DeltaTime(), m_Camera);
			input = true;
		}

		if (KeyBoard::IskeyPressed(window, GLFW_KEY_A))
		{
			m_Player[0].ProcessKeyboard(InputState::P_LEFT, m_Timer.DeltaTime(), m_Camera);
			input = true;
		}
		else if (KeyBoard::IskeyPressed(window, GLFW_KEY_D))
		{
			m_Player[0].ProcessKeyboard(InputState::P_RIGHT, m_Timer.DeltaTime(), m_Camera);
			input = true;
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_SPACE))
		{
			player.StartBlend(DODGE);
			player.m_Unable = true;
			input = true;
		}

		//マウス入力
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			player.StartBlend(BLOCK);
			player.m_Unable = true;
			input = true;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			player.m_Unable = false;
			player.StartBlend(ATTACK1);
			input = true;
		}
		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_1))
		{
			player.StartBlend(IDLE);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_2))
		{
			player.StartBlend(IDLE_2);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_3))
		{
			player.StartBlend(DODGE);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_4))
		{
			player.StartBlend(ATTACK1);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_5))
		{
			player.StartBlend(WALK_BACK);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_6))
		{
			player.StartBlend(RUN_BACK);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_7))
		{
			player.StartBlend(U_TURN);
		}
		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_0))
		{
			player.StartBlend(JUMP_ATT);
		}
	}
	else
	{
		m_CamRotEnable = false;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			m_CamRotEnable = true;
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_1))
		{
			player.PlayAni(IDLE);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_2))
		{
			player.PlayAni(IDLE_2);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_3))
		{
			player.PlayAni(DODGE);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_4))
		{
			player.PlayAni(ATTACK1);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_5))
		{
			player.PlayAni(WALK_BACK);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_6))
		{
			player.PlayAni(RUN_BACK);
		}

		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_7))
		{
			player.PlayAni(U_TURN);
		}
		if (KeyBoard::IskeyTriggered(window, GLFW_KEY_0))
		{
			player.PlayAni(JUMP_ATT);
		}
	}

	//Mキーを押してる時、カメラ移動を禁止する
	if (KeyBoard::IskeyTriggered(window, GLFW_KEY_M))
	{
		m_Input ^= 1;
	}

	if (!input && m_Input)
		m_Player[0].ProcessKeyboard(InputState::P_NONE, m_Timer.DeltaTime(), m_Camera);



}

//　FPS計算
void Game::calculateFPS(GLFWwindow* window) {
	double currentTime = glfwGetTime();
	double elapsedTime = currentTime - lastTime;

	if (elapsedTime >= FPS_REFRSH_INTERVAL) {
		fps = (float)(frameCount / elapsedTime);

		std::stringstream ss;
		ss << "FPS: " << fps << "  " << m_Player[0].GetPos().x << "  " << m_Player[0].GetPos().y << "  " << m_Player[0].GetPos().z;

		glfwSetWindowTitle(window, ss.str().c_str());

		frameCount = 0;
		lastTime = (float)currentTime;
	}

	frameCount++;
}

void Game::ResetLight()
{
	DirLight* light = static_cast<DirLight*>(m_Light[0]);
	light->direction = DIR_DIR;
	light->ambient = DIR_AMIB;
	light->diffuse = DIR_DIFF;
	light->specular = DIR_SPEC;
}

void Game::ReadNodeHierarchyImGui(NodeData& node)
{
	ImGui::PushID(&node);
	Bone* Bone = m_Player[0].m_Animator.GetCurrentAni()->FindBone(node.name);
	size_t lastSlashPos = node.name.find_last_of('_');
	if (Bone)
	{
		//骨情報を階段的に読み込む
		if (ImGui::TreeNodeEx(node.name.substr(lastSlashPos + 1).c_str()))
		{
			//当骨を使用するか？
			ImGui::Checkbox("Use", &node.unmasked);
		
			if (!node.unmasked)
				SeteUseFalse(node);
			for (int i = 0; i < node.childrenCount; i++)
			{
				ReadNodeHierarchyImGui(node.children[i]);
			}
			ImGui::TreePop();
		}
	}
	else if (node.name == "RootNode")
	{
		if (ImGui::TreeNodeEx(node.name.substr(lastSlashPos + 1).c_str()))
		{
			for (int i = 0; i < node.childrenCount; i++)
			{
				ReadNodeHierarchyImGui(node.children[i]);
			}
			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

void Game::SeteUseFalse(NodeData& node)
{
	if (!node.unmasked)
	{
		for (int i = 0; i < node.childrenCount; i++)
		{
			node.children[i].unmasked = false;
			SeteUseFalse(node.children[i]);
		}
	}
	else
	{
		for (int i = 0; i < node.childrenCount; i++)
		{
			SeteUseFalse(node.children[i]);
		}
	}
}

void Game::SeteUseTrue(NodeData& node)
{

	node.unmasked = true;
	for (int i = 0; i < node.childrenCount; i++)
	{
		node.children[i].unmasked = true;
		SeteUseTrue(node.children[i]);
	}

}

void Game::UseAllBones()
{
	SeteUseTrue(m_Player[0].m_Animator.GetCurrentAni()->GetModel()->GetRootNode());

}


void Game::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
