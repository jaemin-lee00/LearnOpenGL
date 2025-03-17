#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>

#include <iostream>

#include <functional>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "shaders/shader_s.h"

//Doucment adress
//
//  Last file update date : 2025-03-17 12:55
//
//  now number, <<theme>> : 10 , Transformations
//  https://heinleinsgame.tistory.com/   -number- , -Theme-
//  sample docu
//
//  Additonal information : Shader Study
/*
*
*/

// Namespace for cleaner code
using namespace std;

// Screen constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// sotres how much we're seeing of either texture
float mixValue = 0.2f;
unsigned int texture1, texture2;


// Shader Source File Directories
const char* vertexShaderPath = "src/shaders/vertexShader.vs";
const char* fragmentShaderPath = "src/shaders/fragmentShader.fs";

// Function declarations
bool init();
bool draw();
void mainLoop();
void cleanup();
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
unsigned int VAO = 0;

// Function declarations for shader compilation and setup
bool setupShader();
bool setupTextureData();
bool setupVertexData();

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

    // Setup Vertex Data
    if (!loggingDecorator(setupVertexData, "setupVertexData")) {
        return false;
    }

    // Setup Texture Data
    if (!loggingDecorator(setupTextureData, "setupTextureData")) {
        return false;
    }

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
	ourShader->setInt("texture1", 0);
	ourShader->setInt("texture2", 1);

	stbi_set_flip_vertically_on_load(false); // reset it to default

    return true;
}

bool setupVertexData() {
    float vertices[] = {

        // location             //Color             //Texture Coords
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right 
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top  Left

    };

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

    glGenBuffers(1, &EBO);
    if (EBO == 0) {
        cout << "[Err :EBO ] > msg :  EBO error" << endl;
        return false;
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


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
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // Animation logic based on time
        //float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        ourShader->setFloat("mixValue", mixValue);

        ourShader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
