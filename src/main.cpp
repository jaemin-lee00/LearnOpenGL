#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 데코레이터 함수 선언
template <typename Func, typename... Args>
auto loggingDecorator(Func func, const std::string& funcName, Args... args) {
    cout << "Calling function: " << funcName << endl;
    auto result = func(args...);
    if (!result) {
        cout << "Error in function: " << funcName << endl;
    } else {
        cout << "Success: " << funcName << endl;
    }
    return result;
}

GLFWwindow* initGLFW();
bool initGLAD();
void mainLoop(GLFWwindow* window);
void clearScreen();

// 전역 상수
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // GLFW 초기화
    auto window = loggingDecorator(initGLFW, "initGLFW");
    if (window == NULL) return -1;

    // GLAD 초기화
    if (!loggingDecorator(initGLAD, "initGLAD")) return -1;

    // 메인 루프
    mainLoop(window);

    glfwTerminate();
    return 0;
}

GLFWwindow* initGLFW() {
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ZMMR", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to open GLFW window" << endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

bool initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return false;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    return true;
}

void mainLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        clearScreen();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void clearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
