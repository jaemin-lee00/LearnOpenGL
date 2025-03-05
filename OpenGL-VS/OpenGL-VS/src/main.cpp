#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // the program ID
    unsigned int ID;


    // constructor raeds and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

}

#endif


#include <GLFW/glfw3.h>
#include <functional>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"


//Doucment adress
//
//  Last file update date : 2025-03-05 23:20
//
//  now number : 9
//  https://heinleinsgame.tistory.com/   -number-
//  sample docu
/*
*
*/

// Namespace for cleaner code
using namespace std;

// Screen constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;      // position variable has attribute position 0
    layout (location = 1) in vec3 aColor;    // color variable has attribute position 1
    layout (location = 2) in vec2 aTexCoord; // texture variable has attribute position 2

    out vec3 ourColor;                      // output a color to the fragment shader
    out vec2 TexCoord;                      // output a texture coordinate to the fragment shader

    void main(){
        gl_Position =  vec4(aPos, 1.0);       // Directly give vec3 to vec4 creator
        ourColor = aColor;                  // Set our color to the input color we got from the vertex data
        TexCoord = aTexCoord;               // Set our texture coordinates to the input texture coordinates we got from the vertex data
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;


    in vec3 ourColor;
    in vec2 TexCoord;

    uniform sampler2D texture1;
    uniform sampler2D texture2;
  
    void main() {
        FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    }
)";


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
unsigned int shaderProgram = 0;
unsigned int VAO = 0;

// Function declarations for shader compilation and setup
bool compileVertexShader(unsigned int& vertexShader);
bool compileFragmentShader(unsigned int& fragmentShader);
bool linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
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
    unsigned int vertexShader = 0, fragmentShader = 0;

    // Compile Vertex Shader
    if (!loggingDecorator(compileVertexShader, "compileVertexShader", ref(vertexShader))) {
        return false;
    }

    // Compile Fragment Shader
    if (!loggingDecorator(compileFragmentShader, "compileFragmentShader", ref(fragmentShader))) {
        return false;
    }

    // Link Shader Program
    if (!loggingDecorator(linkShaderProgram, "linkShaderProgram", vertexShader, fragmentShader)) {
        return false;
    }

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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

// Shader Control
bool compileVertexShader(unsigned int& vertexShader) {
    int success;
    char infoLog[512];

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return false;
    }

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "[Err : Vertex Shader] > msg :  Vertex Shader compilation error: " << infoLog << endl;
        return false;
    }

    return true;
}

bool compileFragmentShader(unsigned int& fragmentShader) {
    int success;
    char infoLog[512];

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        return false;
    }

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "[Err : Fragment Shader] > msg : Fragment Shader compilation error: " << infoLog << endl;
        return false;
    }

    return true;
}

bool linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    int success;
    char infoLog[512];

    // Shader Program
    shaderProgram = glCreateProgram();
    if (shaderProgram == 0) {
        return false;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "[Err : Shader] > msg : Shader Program linking error: " << infoLog << endl;
        return false;
    }

    return true;
}

bool setupTextureData() {

    unsigned int texture1, texture2;

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

        // Draw our triangle
        glUseProgram(shaderProgram);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourTexture");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

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
    glDeleteProgram(shaderProgram);
}
     
// Running process 
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
