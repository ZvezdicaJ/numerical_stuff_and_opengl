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
