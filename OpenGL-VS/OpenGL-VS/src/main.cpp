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
//  Last file update date : 2025-10-02 11:45
//
//  <<theme>> : Color
//  https://learnopengl.com/Lighting/  -Theme-
//  
// Make Color Class
/*  
*   In Color Class , we will learn about how to make color in OpenGL
*   Step1 , Make shader for color                                               V    
*   Step2 , Make Camera class for camera movement                               V
* 
*   Step3 , Make Color Shader                                                   V
*   Step4 , Make Color Cube Shader                                              V
*   Step5, Intergrate Color Classes with main.cpp
*       STEP5.1 : Make Vertex Shader for Color Cube in setupVertexData          now
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

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Light properties
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Position of the light source

// sotres how much we're seeing of either texture
unsigned int texture1, texture2;


// Transformation matrix
glm::mat4 trans = glm::mat4(1.0f);

// Model matrix
glm::mat4 model = glm::mat4(1.0f);
// view matrix
glm::mat4 view = glm::mat4(1.0f);
// projection matrix
glm::mat4 projection = glm::mat4(1.0f);

// Cube positions
// After chage this array to Get CubePositions function
glm::vec3 cubePositions[10];

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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void setModel();
void setView();
void setProjection();
void setTransform(int cubeNum);


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

// Global variables for OpenGL objects
GLFWwindow* window = nullptr;
Shader* ourShader = nullptr;
Shader* lightShader = nullptr;
Shader* lightCubeShader = nullptr;

unsigned int VAO = 0;

// Function declarations for shader compilation and setup
bool setupShader();
bool setupLightShader();
bool setupLightCubeShader();
bool setupTextureData();
bool setupVertexData();

// Function to set the model matrix
void setModel() {
	// Set the model matrix to identity
	model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	ourShader->setMat4("model", model);
}

// Function to set the view matrix
void setView() {
	// Set the view matrix to look at the origin from the camera position
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	ourShader->setMat4("view", view);
}

// Function to set the projection matrix
void setProjection() {
	// Set the projection matrix to a perspective projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader->setMat4("projection", projection);
}

// Function to set the transformation matrix
void setTransform(int cubeNum) {

    // Transformation
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[cubeNum]);

	float angle = 20.0f * cubeNum; // Rotate each cube at a different
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    ourShader->setMat4("model", model);
}

void setCameraTransform() { 
    // Set the camera transformation matrix
	view = camera.GetViewMatrix();

    ourShader->setMat4("view", view);
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
	if (!loggingDecorator(setupShader, "setupShader")) {
		return false;
	}

    // Setup LightShader
    //if (!loggingDecorator(setupLightShader, "setupLightShader")) {
    //	return false;
    //}

    // Setup LightCubeShader
    if (!loggingDecorator(setupLightCubeShader, "setupLightCubeShader")) {
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
bool setupShader() {
    try {
        // Create a shader using shader class
        ourShader = new Shader(vertexShaderPath, fragmentShaderPath);
        return true;
    }
    catch (std::exception& e) {
        cout << "[Err : Shader] > msg : " << e.what() << endl;
        return false;
    }
}

// Setup Light Shader
bool setupLightShader() {
    try {
        // Create a shader using shader class
        lightShader = new Shader(lightVertexShaderPath, lightFragmentShaderPath);
        return true;
    }
    catch (std::exception& e) {
        cout << "[Err : Shader] > msg : " << e.what() << endl;
        return false;
    }
}

// Setup Light Cube Shader
bool setupLightCubeShader() {
    try {
        // Create a shader using shader class
        lightCubeShader = new Shader(lightCubeVertexShaderPath, lightCubeFragmentShaderPath);
        return true;
    }
    catch (std::exception& e) {
        cout << "[Err : Shader] > msg : " << e.what() << endl;
        return false;
    }
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

	// Cube positions
    //cubePositions[0] = glm::vec3(0.0f,  0.0f,  0.0f);
    //cubePositions[1] = glm::vec3(2.0f,  5.0f, -15.0f);
    //cubePositions[2] = glm::vec3(-1.5f, -2.2f, -2.5f);
    //cubePositions[3] = glm::vec3(-3.8f, -2.0f, -12.3f);
    //cubePositions[4] = glm::vec3(2.4f, -0.4f, -3.5f);
    //cubePositions[5] = glm::vec3(-1.7f,  3.0f, -7.5f);
    //cubePositions[6] = glm::vec3(1.3f, -2.0f, -2.5f);
    //cubePositions[7] = glm::vec3(1.5f,  2.0f, -2.5f);
    //cubePositions[8] = glm::vec3(1.5f,  0.2f, -1.5f);
    //cubePositions[9] = glm::vec3(-1.3f,  1.0f, -1.5f);

    unsigned int indices[] = {  // it start from 0!
        0, 1, 3,     // first triangle
        1, 2, 3     // second triangle
    };


    unsigned int VBO = 0;
    unsigned int EBO = 0;

    glGenVertexArrays(1, &VAO);
    if (VAO == 0) {
        cout << "[Err : VAO ] > msg :  VAO error" << endl;
        return false;
    }

    glGenBuffers(1, &VBO);
    if (VBO == 0) {
        cout << "[Err : VBO ] > msg :  VBO error" << endl;
        return false;
    }

    //glGenBuffers(1, &EBO);
    //if (EBO == 0) {
    //    cout << "[Err :EBO ] > msg :  EBO error" << endl;
    //    return false;
    //}



    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

	unsigned int lightCubeVAO = 0;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Texture attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
        ourShader->use();

        setProjection();
        // camera /view transformation
        setCameraTransform();

		// render the container
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            setTransform(i); // Set the transformation for each cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glBindVertexArray(0);


        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


// Ending process
void cleanup() {
    glDeleteVertexArrays(1, &VAO);

	if (ourShader) {
		delete ourShader;
		ourShader = nullptr;
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {


    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }


    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top
    
    
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw : whenever the mouse scroll wheel is used, this function is called
// -----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}