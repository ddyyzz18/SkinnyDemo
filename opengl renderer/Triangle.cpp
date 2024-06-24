//#include <glad/glad.h>
//#include <GLFW\glfw3.h>
//#include <iostream>
//#include "shader.h"
//#include "camera.h"
//#include "stb_image.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include "light.h"
//#include "model_animation.h"
//#include "animation.h"
//#include "animator.h"
//#include "timer.h"
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main()
//{
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//	// glfw window creation
//	// --------------------
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Game", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	// tell GLFW to capture our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//	//stbi_set_flip_vertically_on_load(true);
//
//	// configure global opengl state
//	// -----------------------------
//	glEnable(GL_DEPTH_TEST);
//	// build and compile shaders
//	// -------------------------
//	//Shader ourShader("anime.vs", "anime.fs");
//	//Shader ourShader("normalmap.vs", "normalmap.fs");
//	Shader ourShader("test.vs", "testnormal.fs");
//
//	// load models
//	// -----------
//	Model ourModel("MODEL/cabo/Capoeira.dae");
//	Texture normal = ourModel.loadMaterialTexturesManully("textures/maria_normal.png","MODEL/cabo");
//
//	ourModel.meshes[0].textures.push_back(normal);
//	Animation danceAnimation("MODEL/cabo/Capoeira.dae", &ourModel);
//	/*Model ourModel("MODEL/Emily_FBX_Jump Animation/HN_GrimmChild_Anim_12framesfinal.fbx");
//	Animation danceAnimation("MODEL/Emily_FBX_Jump Animation/HN_GrimmChild_Anim_12framesfinal.fbx", &ourModel);*/
//	Animator animator(&danceAnimation);
//
//	// draw in wireframe
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	//Timer init
//	Timer gametimer;
//	// render loop
//	// -----------
//	while (!glfwWindowShouldClose(window))
//	{
//		static bool first = true;
//		if (first)
//		{
//			gametimer.Reset();
//			first = false;
//		}
//		// per-frame time logic
//		// --------------------
//		gametimer.Tick();
//		//float currentFrame = static_cast<float>(glfwGetTime());
//		//deltaTime = currentFrame - lastFrame;
//		//lastFrame = currentFrame;
//
//		// input
//		// -----
//		processInput(window);
//	
//		animator.UpdateAnimation(gametimer.DeltaTime());
//		//animator.UpdateAnimation(deltaTime);
//	
//
//		// render
//		// ------
//		glClearColor(0.05f, 0.8f, 0.05f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// don't forget to enable shader before setting uniforms
//		ourShader.use();
//
//		//lighting
//	 //   glUniform3f(glGetUniformLocation(ourShader.ID, "light.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//		//glUniform3f(glGetUniformLocation(ourShader.ID, "light.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//		//glUniform3f(glGetUniformLocation(ourShader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
//		//glUniform3f(glGetUniformLocation(ourShader.ID, "light.diffuse"), 0.8f, 0.8f, 0.8f);
//		//glUniform3f(glGetUniformLocation(ourShader.ID, "light.specular"), 0.8f, 0.8f, 0.8f);
//		//glUniform1f(glGetUniformLocation(ourShader.ID, "light.constant"), 1.0f);
//		//glUniform1f(glGetUniformLocation(ourShader.ID, "light.linear"), 0.09);
//		//glUniform1f(glGetUniformLocation(ourShader.ID, "light.quadratic"), 0.032);
//		//glUniform1f(glGetUniformLocation(ourShader.ID, "light.cutOff"), glm::cos(glm::radians(12.5f)));
//		//glUniform1f(glGetUniformLocation(ourShader.ID, "light.outerCutOff"), glm::cos(glm::radians(25.0f)));
//		glm::vec3 lightPos(0.0f, 100.0f, 100.0f);
//		glm::vec3 lightDir(0.0f, -100.0f,0.0f);
//		ourShader.setVec3("light.direction", lightDir);
//		ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
//		ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
//		ourShader.setVec3("light.specular", 0.8f, 0.8f, 0.8f);
//		// lightpos
//		//ourShader.setVec3("lightPos", camera.Position);
//		// viewpos
//		ourShader.setVec3("viewPos", camera.Position);
//		// view/projection transformations
//		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		glm::mat4 view = camera.GetViewMatrix();
//		ourShader.setMat4("projection", projection);
//		ourShader.setMat4("view", view);
//
//		auto transforms = animator.GetFinalBoneMatrices();
//		for (int i = 0; i < transforms.size(); ++i)
//			ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
//
//	/*	for (int i = 0; i < transforms.size(); ++i)
//		{
//			glm::mat4 temptrans;
//			temptrans = glm::transpose(glm::inverse(transforms[i]));
//			ourShader.setMat4("normalBonesMatrices[" + std::to_string(i) + "]", temptrans);
//		}*/
//		// render the loaded model
//		//glm::mat4 model = glm::mat4(1.0f);
//		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
//		//ourShader.setMat4("model", model);
//		//glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));
//		//ourShader.setMat3("normal", normal);
//		//ourModel.Draw(ourShader);
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
//		//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
//	/*	glm::mat4 normal = glm::transpose(glm::inverse(model));
//		ourShader.setMat4("normal", normal);*/
//		/*glm::mat4 normal = glm::mat4(glm::transpose(glm::inverse(model)));
//		ourShader.setMat4("normal", normal);*/
//		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
//		ourShader.setMat4("model", model);
//		ourModel.Draw(ourShader);
//
//		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//		// -------------------------------------------------------------------------------
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//	}
//
//	// glfw: terminate, clearing all previously allocated GLFW resources.
//	// ------------------------------------------------------------------
//	glfwTerminate();
//	return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
