#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <../shader_s.h>
#include <../camera.h>
#include <../model.h>
#include <../GraphNode.h>
#include <iostream>
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool showGrid = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// lighting
glm::vec3 lightPos(0.f, 10.0f,0.0f);
glm::vec3 lightPos2(0.f, 14.0f, 10.0f);
glm::vec3 PointLight(10.f, 6.0f, 3.0f);
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//GLFW_CURSOR_NORMAL   GLFW_CURSOR_DISABLED
	glfwSwapInterval(1); // Enable vsync
   // Initialize OpenGL loader
	bool err = false;
	#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	err = gl3wInit() != 0;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	err = glewInit() != GLEW_OK;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	#endif
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*IMGUI_CHECKVERSION();*/
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();



	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//load textures
	//unsigned int diffuseMap = loadTexture("../../res/textures/container2.png");
	//unsigned int specularMap = loadTexture("../../resources/textures/container2_specular.png");


	// build and compile shaders
	// -------------------------
	Shader lightingShader("../../res/shaders/zad4.vs", "../../res/shaders/zad4.fs"); //"../../res/shaders/6.multiple_lights.vs", "../../res/shaders/6.multiple_lights.fs"
	Shader lampShader("../../res/shaders/2.2.lamp.vs", "../../res/shaders/2.2.lamp.fs");



	lightingShader.use();
	lampShader.use();



	// load models
	// -----------

	 


	Mesh *planeMesh = new Mesh();
	//planeMesh->generatePlaneWithNormalsAndTextureCords(20);
	planeMesh->generatePlaneWithNormalsAndTextureCords(20);
	TextureEnum en;
	planeMesh->AddTexture(TextureEnum::texture_diffuse, "res/textures/container2.png");
	planeMesh->AddTexture( TextureEnum::texture_specular, "res/textures/container2_specular.png");
	Model *plane = new Model(planeMesh, false);
	


	Mesh *testBox = new Mesh();

	testBox->generateBoxWithNormalsAndTextureCords();
	Model *LightModelT = new Model(testBox);
	Model *nanosuit = new Model(("res/models/nanosuit/nanosuit.obj"));




	plane->SetShader(&lightingShader);
	LightModelT->SetShader(&lampShader);
	nanosuit->SetShader(&lightingShader);
	
	//ustawienie transformu dla modeli
	unsigned int amount = 6;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];



	for (unsigned int i = 0;i < amount;i++) {
	
		modelMatrices[i] = glm::mat4(1);
		modelMatrices[i] = glm::scale(modelMatrices[i], glm::vec3(0.5f, 0.5f, 0.5f));
		modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(-20, 0, 0));

	}
	for (unsigned int i = 0;i < amount/2;i++) {
		modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3((float)i*20, 0, 0));
	}
	for (unsigned int i = amount/2;i < amount;i++) {
		modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(((float)i- amount/2)*20, 0, 20));
	}
	nanosuit->setInstantiate(modelMatrices, amount);


	glm::mat4* LightModels;
	LightModels = new glm::mat4[3];
	for (unsigned int i = 0;i < 3;i++) {

		LightModels[i] = glm::mat4(1);
	
	}
	LightModels[0] = glm::translate(LightModels[0], lightPos);
	LightModels[1] = glm::translate(LightModels[1], lightPos2);
	LightModels[2]= glm::translate(LightModels[2], PointLight);
	/*for (unsigned int i = 0;i < 3;i++) {

		LightModels[i] = glm::scale(LightModels[i], glm::vec3(0.3f, 0.3f, 0.3f));

	}*/



	LightModelT->setInstantiate(LightModels,3);


	glm::mat4* PlaneModel;
	PlaneModel = new glm::mat4[1];
	PlaneModel[0] = glm::mat4(1);
	PlaneModel[0] = glm::translate(PlaneModel[0], glm::vec3(0, -1, 0));
	plane->setInstantiate(PlaneModel, 1);

	


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	GraphNode *World = new GraphNode(false);
	GraphNode *nanosuitNode = new GraphNode(true, nanosuit);
	GraphNode *lights = new GraphNode(true, LightModelT);
	GraphNode *planeG = new GraphNode(true, plane);

	World->AddChild(nanosuitNode);
	World->AddChild(lights);
	World->AddChild(planeG);
	World->Update(0);






	//World->SetTransform(new glm::mat4(1));










	// render loop
	// -----------
	bool pointLightFlag = true;
	bool spotLight1Flag = true;
	bool spotLight2Flag = true;
	bool dirLightFlag = true;
	float DirX = -0.2f;
	float DirY = -1.0f;
	float DirZ = -0.3f;
	float ambient[3] = { 0.7f, 0.7f, 0.7f };
	float specular[3] = { 0.7f, 0.7f, 0.7f };
	float diffuse[3]= { 0.7f, 0.7f, 0.7f };
	float constant = 1.0f;
	float linear = 0.01;
	float quadratic = 0.01;
	bool *k=new bool(true);
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// per-frame time logic
	// --------------------



		ImGui::Begin("Change lights", k, ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::Checkbox("point Light", &pointLightFlag);
			ImGui::Checkbox("spot Light1", &spotLight1Flag);
			ImGui::Checkbox("spot Light2", &spotLight2Flag);
			ImGui::Checkbox("directional lights", &dirLightFlag);

			ImGui::SliderFloat("x of DirectionalLight", &DirX, -1, 1);
			ImGui::SliderFloat("y of DirectionalLight", &DirY, -1, 1);
			ImGui::SliderFloat("z of DirectionalLight", &DirZ, -1, 1);
			ImGui::Text("To disable mouse press 'K', to enable mouse press 'L'");
			ImGui::Text("To stop moving camera with mouse press 'space'");
			ImGui::End();
		}
		ImGui::Begin("Change params of PointLight", k, ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::SliderFloat3("diffuse", diffuse, 0.0f, 1.0f);
			ImGui::SliderFloat3("ambient", ambient, 0.0f, 1.0f);
			ImGui::SliderFloat3("specular", specular, 0.0f, 1.0f);
			ImGui::SliderFloat("constant", &constant, 0.0f, 1.0f);
			ImGui::SliderFloat("linear", &linear, 0.0f, 1.0f);
			ImGui::SliderFloat("quadratic", &quadratic, 0.0f, 1.0f);
			ImGui::End();
		}

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//// Start the Dear ImGui frame
		// input
		// -----
		PointLight.x=PointLight.x+sin(glfwGetTime()/2)/10;
		LightModels[2] = glm::translate(LightModels[2], glm::vec3(sin(glfwGetTime()/2) / 10,0, 0));
		//LightModelT->setInstantiate(LightModels, 3);
		processInput(window);

		// render
		// ------
		glClearColor(0.08f, 0.085f, 0.085f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("shininess", 120.0f);
		// directional light
		lightingShader.setBool("dirLights[0].flag", dirLightFlag);

		lightingShader.setVec3("dirLights[0].direction", DirX, DirY, DirZ);
		lightingShader.setVec3("dirLights[0].ambient", 0.01f, 0.01f,0.01f);
		lightingShader.setVec3("dirLights[0].diffuse", 0.3f, 0.3f, 0.3f);
		lightingShader.setVec3("dirLights[0].specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setBool("pointLights[0].flag", pointLightFlag);
		lightingShader.setVec3("pointLights[0].position", PointLight);
		lightingShader.setVec3("pointLights[0].ambient", ambient[0], ambient[1], ambient[2]);
		lightingShader.setVec3("pointLights[0].diffuse", diffuse[0], diffuse[1], diffuse[2]);
		lightingShader.setVec3("pointLights[0].specular", specular[0], specular[1], specular[2]);
		lightingShader.setFloat("pointLights[0].constant", constant);
		lightingShader.setFloat("pointLights[0].linear", linear);
		lightingShader.setFloat("pointLights[0].quadratic", quadratic);





		// spotLight
		lightingShader.setBool("spotLights[0].flag", spotLight1Flag);
		lightingShader.setVec3("spotLights[0].position", lightPos);
		lightingShader.setVec3("spotLights[0].direction", glm::vec3(-LightModels[0][1][0], -LightModels[0][1][1], -LightModels[0][1][2])); //lightPos in models
		lightingShader.setVec3("spotLights[0].ambient", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLights[0].diffuse", 0.0f, 0.0f, 1.0f);
		lightingShader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLights[0].constant", 1.0f); //czym mniejsze tym światło jest bardziej "intensywne" 
		lightingShader.setFloat("spotLights[0].linear", 0.02); //zanikanie
		lightingShader.setFloat("spotLights[0].quadratic", 0.02);
		lightingShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));
		
		//spotlight2
		lightingShader.setBool("spotLights[1].flag", spotLight2Flag);
		lightingShader.setVec3("spotLights[1].position", lightPos2);
		lightingShader.setVec3("spotLights[1].direction", glm::vec3(-LightModels[1][1][0], -LightModels[1][1][1], -LightModels[1][1][2]));
		lightingShader.setVec3("spotLights[1].ambient", 0.6f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLights[1].diffuse", 1.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLights[1].constant", 1.0f);
		lightingShader.setFloat("spotLights[1].linear", 0.02);
		lightingShader.setFloat("spotLights[1].quadratic", 0.02);
		lightingShader.setFloat("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(15.0f)));
		
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		lampShader.use();

		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		
		World->Update(0);
		World->Draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) //za pomocą K włączasz myszkę
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//GLFW_CURSOR_NORMAL   GLFW_CURSOR_DISABLED
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//GLFW_CURSOR_NORMAL   GLFW_CURSOR_DISABLED

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {

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

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

struct SpotLight {
	bool flag;
};

