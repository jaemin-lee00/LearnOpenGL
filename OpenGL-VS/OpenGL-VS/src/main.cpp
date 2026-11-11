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
//  Last file update date : 2026-11-11 01:35
//
//  <<theme>> : Advanced GLSL
//  https://learnopengl.com/Advanced-OpenGL/  -Theme-
// 
/*  
*   Done : Uniform Block and buffer ,  in Advanced GLSL, Uniform Buffer Object in Advanced GLSL,
*   Todo : Read the examples and write the code,  in Advanced GLSL,
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
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(vector<std::string> faces);

// Screen constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Initial camera position
float lastX = SCR_WIDTH / 2.0f; // Last X position of the mouse
float lastY = SCR_HEIGHT / 2.0f; // Last Y position of the mouse
bool firstMouse = true; // Flag to check if it's the first mouse input

float deltaTime = 0.0f; // Time between current frame and last framezz
float lastFrame = 0.0f; // Time of last frame

// Model matrix
glm::mat4 model = glm::mat4(1.0f);
// view matrix
glm::mat4 view = glm::mat4(1.0f);
// projection matrix
glm::mat4 projection = glm::mat4(1.0f);

// Global variables for OpenGL objects
GLFWwindow* window = nullptr;

//In this case, we use this variable to store the shader program ID, which is used to reference the compiled shader program in OpenGL.
// sotres how much we're seeing of either texture (naming Teuxter ID) refectoring should be done frequently depending on the situation
unsigned int cubeTexture;
unsigned int cubeVAO, cubeVBO;
unsigned int uboMatrices; // Uniform Buffer Object for matrices

Shader* shaderRed = nullptr, *shaderGreen = nullptr, *shaderBlue = nullptr, *shaderYellow = nullptr;

const char* vertexShaderPath = "src/shaders/advanced_glsl.vs";

const char* fragmentShaderRedPath = "src/shaders/red.fs";
const char* fragmentShaderGreenPath = "src/shaders/green.fs";
const char* fragmentShaderBluePath = "src/shaders/blue.fs";
const char* fragmentShaderYellowPath = "src/shaders/yellow.fs";


// Function declarations
bool init();
bool draw();
bool setupVertexData();
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

    // Drawing Setup
    if (!loggingDecorator(draw, "draw")) {
        return -1;
    }

    // Main Render Loop
    mainLoop();


    glfwTerminate();
    return 0;
}

// Init process
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

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    return true;
}

// Draw process
bool draw() {
    // Setup Shader
    shaderRed = new Shader(vertexShaderPath, fragmentShaderRedPath);
    shaderGreen = new Shader(vertexShaderPath, fragmentShaderGreenPath);
    shaderBlue = new Shader(vertexShaderPath, fragmentShaderBluePath);
    shaderYellow = new Shader(vertexShaderPath, fragmentShaderYellowPath);

	// Setup Vertex Data
	if (!loggingDecorator(setupVertexData, "setupVertexData")) {
		return false;
	}

	// Setup Texture Data
	//cubeTexture = loggingDecorator(loadTexture, "loadTexture", texturePath);
	//if (!cubeTexture) {
	//	return false;
	//}


	return true;
}

bool setupVertexData() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions         
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

    // cube VAO
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed->ID, "Matrices");
	unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen->ID, "Matrices");
	unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue->ID, "Matrices");
	unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow->ID, "Matrices");

	glUniformBlockBinding(shaderRed->ID, uniformBlockIndexRed, 0);
	glUniformBlockBinding(shaderGreen->ID, uniformBlockIndexGreen, 0);
	glUniformBlockBinding(shaderBlue->ID, uniformBlockIndexBlue, 0);
	glUniformBlockBinding(shaderYellow->ID, uniformBlockIndexYellow, 0);

	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void mainLoop() {
    
	while (!glfwWindowShouldClose(window)) {

		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindVertexArray(cubeVAO);
		
        shaderRed->use();
		glm::mat4 modelRed = glm::mat4(1.0f);
		modelRed = glm::translate(modelRed, glm::vec3(-0.75f, 0.75f, 0.0f));
		shaderRed->setMat4("model", modelRed);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shaderBlue->use();
		glm::mat4 modelBlue = glm::mat4(1.0f);
		modelBlue = glm::translate(modelBlue, glm::vec3(0.75f, 0.75f, 0.0f));
		shaderBlue->setMat4("model", modelBlue);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shaderGreen->use();
		glm::mat4 modelGreen = glm::mat4(1.0f);
		modelGreen = glm::translate(modelGreen, glm::vec3(-0.75f, -0.75f, 0.0f));
		shaderGreen->setMat4("model", modelGreen);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shaderYellow->use();
		glm::mat4 modelYellow = glm::mat4(1.0f);
		modelYellow = glm::translate(modelYellow, glm::vec3(0.75f, -0.75f, 0.0f));
		shaderYellow->setMat4("model", modelYellow);
		glDrawArrays(GL_TRIANGLES, 0, 36);



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

unsigned int loadTexture(char const* path) {

	if (!path || !*path) {
		cout << "[Err] > msg : Invalid texture path" << endl;
		return 0;
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (!data) {
		cout << "[Err] > msg : Failed to load texture at path: " << path << endl;
		return 0;
	}

	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return textureID;
}

unsigned int loadCubemap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = 0;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
