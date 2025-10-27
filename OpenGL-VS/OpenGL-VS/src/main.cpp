#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>

#include <functional>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "shaders/shader_s.h"
#include "camera.h"

#include <iostream>

// Document adress
//
//  Last file update date : 2025-10-26 23:20
// 
//  <<theme>> : Materials
//  https://learnopengl.com/Lighting/  -Theme-
//  
/*  
*   Done Materials Chapter
*   UnderStand Lighting maps & Diffuse maps
*
*   Problems to be solved :-----------------------------------------------
* 
*   File segmentation of integrated documents for purpose
* 
* 
* 
*/


// Namespace for cleaner code
using namespace std;

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

// Light properties
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Position of the light source

// sotres how much we're seeing of either texture
unsigned int texture1, texture2;

// Global variables for OpenGL objects
GLFWwindow* window = nullptr;
Shader* ourShader = nullptr;
Shader* lightingShader = nullptr;
Shader* lightCubeShader = nullptr;

unsigned int cubeVAO = 0;
unsigned int lightCubeVAO = 0;

// Model matrix
glm::mat4 model = glm::mat4(1.0f);
// view matrix
glm::mat4 view = glm::mat4(1.0f);
// projection matrix
glm::mat4 projection = glm::mat4(1.0f);

// Shader Source File Directories
const char* vertexShaderPath = "src/shaders/vertexShader.vs";
const char* fragmentShaderPath = "src/shaders/fragmentShader.fs";
    
// LightingShader Source File Directories
const char* lightVertexShaderPath = "src/shaders/basic_lighting.vs";
const char* lightFragmentShaderPath = "src/shaders/basic_lighting.fs";

// LightCubeShader Source File Directories
const char* lightCubeVertexShaderPath = "src/shaders/light_cube.vs";
const char* lightCubeFragmentShaderPath = "src/shaders/light_cube.fs";

// Function declarations
bool init();
bool draw();
void mainLoop();
void cleanup();

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void setModel(Shader* shader);
void setModelTransform(Shader* shader);
void setProjection(Shader* shader);
void setCameraTransform(Shader* shader);

// Function declarations for shader compilation and setup
bool setupShaderUnified(Shader*& shaderPtr, const char* vertexPath, const char* fragmentPath, const std::string& shaderName);
bool setupAllShaders();
bool setupTextureData();
bool setupVertexData();

// Decorator function for error handling
template <typename Func, typename... Args>
auto loggingDecorator(Func func, const std::string& funcName, Args... args) {
    cout << "[Call] > msg : Calling function: " << funcName << endl;
    auto result = func(args...);
    if (!result) {
        cout << "[Err] > msg : Error in function: " << funcName << endl;
    }
    else {
        cout << "[LOG] > msg : Success: " << funcName << endl;
    }
    return result;
}

void setModel(Shader* shader) {
    if (!shader) {
        cout << "[Err] > msg : Shader is null in setModel" << endl;
        return;
    }
    // Set the model matrix to identity (no transformations)
    model = glm::mat4(1.0f);
    shader->setMat4("model", model);
}

// Function to set the view matrix
void setModelTransform(Shader* shader) {
    if (!shader) {
        cout << "[Err] > msg : Shader is null in setView" << endl;
        return;
	}

    // Set the view matrix to look at the origin from the camera position
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    shader->setMat4("model", model);
}

// Function to set the projection matrix
void setProjection(Shader* shader) {
    if(!shader) {
        cout << "[Err] > msg : Shader is null in setProjection" << endl;
        return;
	}

	// Set the projection matrix to a perspective projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->setMat4("projection", projection);
}

void setCameraTransform(Shader* shader) {
    if (!shader) {
        cout << "[Err] > msg : Shader is null in setCameraTransform" << endl;
        return;
	}
    // Set the camera transformation matrix
	view = camera.GetViewMatrix();
    shader->setMat4("view", view);
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

    // Clean up resources
    cleanup();

    glfwTerminate();
    return 0;
}

// Init process
bool init() {
    // Initialize GLFW
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Application", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    return true;
}

// Draw process
bool draw() {

	// Setup Shader
	if (!loggingDecorator(setupAllShaders, "setupAllShader")) {
		return false;
	}

    // Setup Vertex Data
    if (!loggingDecorator(setupVertexData, "setupVertexData")) {
        return false;
    }

    // Setup Texture Data
    //if (!loggingDecorator(setupTextureData, "setupTextureData")) {
    //    return false;
    //}

    return true;
}

// Setup Shader
bool setupShaderUnified(Shader*& shaderPtr, const char* vertexPath, const char* fragmentPath, const std::string& shaderName){

    try {
		// If shader already exists, delete it
        if (shaderPtr) {
            delete shaderPtr;
            shaderPtr = nullptr;
        }

		// Create a shader using shader class
        shaderPtr = new Shader(vertexPath, fragmentPath);
        cout << "[LOG] > msg : " << shaderName << " shader setup successful" << endl;
        return true;
    }
    catch (std::exception& e) {
        cout << "[Err : " << shaderName << " Shader] > msg : " << e.what() << endl;
        return false;
    }
}

bool setupAllShaders() {
    bool success = true;

    // Main ¼ÎÀÌ´õ ¼³Á¤
    if (!loggingDecorator([&]() {
        return setupShaderUnified(ourShader, vertexShaderPath, fragmentShaderPath, "Main");
        }, "setupMainShader")) {
        success = false;
    }

    // Lighting ¼ÎÀÌ´õ ¼³Á¤
    if (!loggingDecorator([&]() {
        return setupShaderUnified(lightingShader, lightVertexShaderPath, lightFragmentShaderPath, "Lighting");
        }, "setupLightingShader")) {
        success = false;
    }

    // LightCube ¼ÎÀÌ´õ ¼³Á¤
    if (!loggingDecorator([&]() {
        return setupShaderUnified(lightCubeShader, lightCubeVertexShaderPath, lightCubeFragmentShaderPath, "LightCube");
        }, "setupLightCubeShader")) {
        success = false;
    }

    return success;
}

bool setupTextureData() {

    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    glActiveTexture(GL_TEXTURE0); // activate the texture unit first  before binding texture
	glBindTexture(GL_TEXTURE_2D, texture1);

    // set the texture wrapping parameters//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    unsigned char* data_container = stbi_load("img/container.jpg", &width, &height, &nrChannels, 0);

    if (data_container) {
        cout << "[LOG] > msg : Texture container loaded successfully" << endl;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_container);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "[Err : Texture] > msg : Failed to load texture" << endl;
        return false;
    }
    stbi_image_free(data_container);


    glActiveTexture(GL_TEXTURE1); // activate the texture unit second before binding texture
    glBindTexture(GL_TEXTURE_2D, texture2);

    // set the texture wrapping parameters//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    unsigned char* data_awesomeface = stbi_load("img/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data_awesomeface) {
        cout << "[LOG] > msg : Texture awesomeface loaded successfully" << endl;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_awesomeface);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "[Err : Texture] > msg : Failed to load texture" << endl;
        return false;
    }
    stbi_image_free(data_awesomeface);


    // set uniform value
	ourShader->use();
	ourShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	ourShader->setVec3("lightColor",  1.0f, 1.0f, 1.0f);

	stbi_set_flip_vertically_on_load(false); // reset it to default

    return true;
}

bool setupVertexData() {
	// Set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VBO = 0;
    unsigned int EBO = 0;

    glGenVertexArrays(1, &cubeVAO);
    if (cubeVAO == 0) {
        cout << "[Err : VAO ] > msg :  VAO error" << endl;
        return false;
    }

    glGenBuffers(1, &VBO);
    if (VBO == 0) {
        cout << "[Err : VBO ] > msg :  VBO error" << endl;
        return false;
    }



    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // Vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/// Maybe this change to 6 * sizeof(float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VBO as it's no longer needed (optional)
    // glDeleteBuffers(1, &VBO);

    // Uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    return true;
}

void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        
		// per-frame time logic
		// -------------------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;   // calculate time difference between current frame and last frame
		lastFrame = currentFrame;               // set last frame to current frame
        
        // Input
        processInput(window);

		// Depth
		glEnable(GL_DEPTH_TEST);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the cube
        lightingShader->use();
		lightingShader->setVec3("light.position", lightPos);
		lightingShader->setVec3("viewPos", camera.Position);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

        lightingShader->setVec3("light.ambient", ambientColor);
        lightingShader->setVec3("light.diffuse", diffuseColor); // darken diffuse light a bit
        lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);


        // material properties
        lightingShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightingShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lightingShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        lightingShader->setFloat("material.shininess", 32.0f);

        setProjection(lightingShader);
        setCameraTransform(lightingShader);
		setModel(lightingShader);

        // Render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render the light cube
        lightCubeShader->use();

		setProjection(lightCubeShader);
		setCameraTransform(lightCubeShader);
		setModelTransform(lightCubeShader);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, lightPos);
        //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        //lightCubeShader->setMat4("model", model);



        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// Ending process
void cleanup() {
    glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);

	if (lightCubeShader) {
		delete ourShader;
		ourShader = nullptr;
	}

    if(lightCubeShader){
        delete lightCubeShader;
        lightCubeShader = nullptr;
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