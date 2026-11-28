#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>

#include "shaders/shader_s.h"
#include "camera.h"
#include "model.h"

#include <iostream>
// Document adress
//
//  Last file update date : 2026-11-28 23:15
//
//  <<theme>> : Shadows
//  https://learnopengl.com/Advanced-Lighting/  -Theme-
// 
/*  
*   Done : The depth map
*   Todo : Rendering shadows
*
*   Problems to be solved :-----------------------------------------------
*
*   
*/

// Namespace for cleaner code
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const * path, bool gammaCorrection);

// Screen constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool gammaEnalbed = false;
bool gammaKeyPressed = false;


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Initial camera position
float lastX = (float) SCR_WIDTH / 2.0f; // Last X position of the mouse
float lastY = (float) SCR_HEIGHT / 2.0f; // Last Y position of the mouse
bool firstMouse = true; // Flag to check if it's the first mouse input

float deltaTime = 0.0f; // Time between current frame and last framezz
float lastFrame = 0.0f; // Time of last frame

// Global variables for OpenGL objects
GLFWwindow* window = nullptr;

// Function declarations
bool init();
void mainLoop();

// Decorator function for error handling
template <typename Func, typename... Args>
auto loggingDecorator(Func func, const std::string& funcName, Args... args) {
    cout << "[Call] > msg : Calling function: " << funcName << endl;
    auto result = func(args...);
    if (!result) {
        cout << "[Err] > msg : " << funcName << " failed" << endl;
    } else {
        cout << "[LOG] > msg : Success: " << funcName << endl;
    }
    return result;
}

int main() {

    // Initialization
    if (!loggingDecorator(init, "init")) {
        return -1;
    }

    // Main Render Loop
    mainLoop();


    glfwTerminate();
    return 0;
}

bool init() {

    // glfw : initialize and configure
    if (!glfwInit()) {
        cout << "[Err] > msg : Failed to initialize GLFW" << endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        cout << "[Err] > msg : Failed to create GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "[Err] > msg : Failed to initialize GLAD" << endl;
        return false;
    }

    return true;
}

void mainLoop() {
    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader("src/shaders/advanced_lighting.vs", "src/shaders/advanced_lighting.fs");
	
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};

	unsigned int planeVAO, planeVBO;

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);


	unsigned int floorTexture = loadTexture("resources/textures/wood.png", false);
	unsigned int floorTextureGammaCorrected = loadTexture("resources/textures/wood.png", true);

	shader.use();
	shader.setInt("floorTexture", 0);

	glm::vec3 lightPositions[] = {
		glm::vec3(-3.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f)
	};
	glm::vec3 lightColors[] = {
		glm::vec3(0.25),
		glm::vec3(0.50),
		glm::vec3(0.75),
		glm::vec3(1.00)
	};

	while (!glfwWindowShouldClose(window)) {

		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		glUniform3fv(glGetUniformLocation(shader.ID, "lightPositions"), 4, &lightPositions[0][0]);
		glUniform3fv(glGetUniformLocation(shader.ID, "lightColors"), 4, &lightColors[0][0]);
		shader.setVec3("viewPos", camera.Position);
		shader.setInt("gamma", gammaEnalbed);


		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gammaEnalbed ? floorTextureGammaCorrected : floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		std::cout << (gammaEnalbed ? "Gamma enabled" : "Gamma disabled") << std::endl;

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

  
// Running process 
void processInput(GLFWwindow* window) {
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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !gammaKeyPressed)
	{
		gammaEnalbed = !gammaEnalbed;
		gammaKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw : whenever the mouse moves, this function is called
// -----------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw : whenever the mouse scroll wheel is used, this function is called
// -----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, bool gammaCorrection) {

	if (!path || !*path) {
		cout << "[Err] > msg : Invalid texture path" << endl;
		return 0;
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data) {

		GLenum internalFormat;
		GLenum dataFormat;

		if (nrChannels == 1) {
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrChannels == 3) {
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4) {
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		cout << "[Err] > msg : Failed to load texture at path: " << path << endl;
		stbi_image_free(data);
	}

	return textureID;
}

