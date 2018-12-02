#include "opengl_test.hpp"

bool check_vertex_shader(const unsigned shader_) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

bool check_fragment_shader(const unsigned shader_) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

bool check_shader_program(const unsigned shaderProgram) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

void pause() {
    bool run = true;
    while (run) {
        if (std::cin.get()) run = false;
    }
}

void OnPlusPressed(GLFWwindow *window) {
    // glfwGetKey function takes the window as input together with a
    // key. The function returns whether this key is currently being pressed.
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width * 1.1, height * 1.1);
    }
}

void OnMinusPressed(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width / 1.1, height / 1.1);
    }
}

void OnEnterPressed(GLFWwindow *window, unsigned &enter_count) {
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (enter_count == 1) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetWindowPos(window, 100, 100);
        }
        if (enter_count == 2) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetWindowPos(window, 100, 100);
        }
        enter_count++;
    }
}

void OnClosePressed(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
