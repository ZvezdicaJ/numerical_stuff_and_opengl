#pragma once

#include <array>
#include <string>
#include <iostream>
#include <QOpenGLFunctions>
enum class RENDER_TYPE {
    UNIFORM_COLOR = 0,
    CUSTOM_COLOR = 1,
    CUSTOM = -1
};
#include "shaders.hpp"

/**
 * @class Shader
 * @brief Class for shaders. This class compiles three shaders, one
 * for each dimension.
 * @details Class can either be UNIFORM_COLOR or CUSTOM_COLOR class.
 * The difference is in shaders they compile. The first one compiles
 * shader which assigns a single color to all vertexes. The latter
 * assigns seprate color to each vertex.
 */
template <RENDER_TYPE T>
class Shader {
  protected:
    unsigned shader_program[3]; /**< shader program array depending
                                  on which vertex size is chosen*/
    bool shaders_compiled[3] = {
        false, false, false}; /**< true/false depending on whether
                                 the shaders were compiled*/
    QOpenGLFunctions *f;

    std::array<std::string, 3> vertex_source;
    std::string fragment_source;
    void compile_shaders();
    /**
     * @brief check if vertex shader has been successfully compiled
     * @param shader_ an unsigned integer referring to vertex shader
     */
    bool check_vertex_shader(const unsigned shader_) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];
        f->glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

        if (!success) {
            f->glGetShaderInfoLog(shader_, 512, NULL, infoLog);
            std::cout
                << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief check if fragment shader has been successfully
     * compiled
     * @param shader_ an unsigned integer referring to fragment
     * shader
     */
    bool check_fragment_shader(const unsigned shader_) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];
        f->glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

        if (!success) {
            f->glGetShaderInfoLog(shader_, 512, NULL, infoLog);
            std::cout
                << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief check if shader program has been successfully linked
     * @param shaderProgram an unsigned integer referring to shader
     * program
     */
    bool check_shader_program(const unsigned shaderProgram) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];

        f->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            f->glGetProgramInfoLog(shaderProgram, 512, NULL,
                                   infoLog);
            std::cout
                << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

  public:
    Shader(QOpenGLFunctions *f_, bool initialize = false) : f(f_) {
        if (initialize) {
            if (RENDER_TYPE::UNIFORM_COLOR == T) {
                vertex_source = shaders::uniform_vertex_shaders;
                fragment_source = shaders::uniform_fragment_shader;
            } else if (RENDER_TYPE::CUSTOM_COLOR == T) {
                vertex_source = shaders::custom_vertex_shaders;
                fragment_source = shaders::custom_fragment_shader;
            }
            compile_shaders();
        }
    }

    void set_opengl_function_pointer(QOpenGLFunctions *f_) {
        f = f_;
    };

    Shader(QOpenGLFunctions *f_,
           const std::array<std::string, 3> vertex_source_,
           const std::string fragment_source_)
        : f(f_), vertex_source(vertex_source_),
          fragment_source(fragment_source_) {
        compile_shaders();
    }

    Shader(Shader &&) = delete;
    Shader &operator=(Shader &&) = delete;
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    unsigned get_shader_program(int i) { return shader_program[i]; }

    void delete_program() {
        for (int i = 0; i < 3; i++)
            f->glDeleteProgram(shader_program[i]);
    }

    void reinitialize_shader_program(std::string vertex_source_,
                                     std::string fragment_source_) {
        if (shaders_compiled[0])
            f->glDeleteProgram(shader_program[0]);
        if (shaders_compiled[1])
            f->glDeleteProgram(shader_program[1]);
        if (shaders_compiled[2])
            f->glDeleteProgram(shader_program[2]);

        vertex_source = vertex_source_;
        fragment_source = fragment_source_;
        compile_shaders();
    };
};
/**
 * @brief Constructor compiles shaders and tests them.
 */
template <>
inline void Shader<RENDER_TYPE::UNIFORM_COLOR>::compile_shaders() {

    if (fragment_source.compare("") == 0) {
        std::cerr << "fragment shader not provided!";
        exit(1);
    }
    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    const char *frag = fragment_source.c_str();
    f->glShaderSource(fragmentShader, 1, &frag, NULL);
    f->glCompileShader(fragmentShader);
    this->check_fragment_shader(fragmentShader);

    for (int i = 0; i < 3; i++) {
        if (vertex_source[i].compare("") == 0)
            continue;
        // create empty shader
        unsigned int vertexShader;
        vertexShader = f->glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        // const char *source =
        // shaders::uniform_vertex_shaders[i].c_str(); std::cout <<
        // source << std::endl;
        const char *vertex = vertex_source[i].c_str();
        f->glShaderSource(vertexShader, 1, &vertex, NULL);
        f->glCompileShader(vertexShader);

        // check if successfully compiled
        this->check_vertex_shader(vertexShader);

        shader_program[i] = f->glCreateProgram();
        f->glAttachShader(shader_program[i], vertexShader);
        f->glAttachShader(shader_program[i], fragmentShader);
        f->glLinkProgram(shader_program[i]);
        this->check_shader_program(shader_program[i]);
        shaders_compiled[i] = true;
        // glUseProgram(shaderProgram);
        f->glDeleteShader(vertexShader);
    }
    f->glDeleteShader(fragmentShader);
}

/**
 * @brief Constructor compiles shaders and tests them.
 */
template <>
inline void Shader<RENDER_TYPE::CUSTOM_COLOR>::compile_shaders() {
    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    const char *frag = fragment_source.c_str();
    f->glShaderSource(fragmentShader, 1, &frag, NULL);
    f->glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    for (int i = 0; i < 3; i++) {
        // create empty shader
        unsigned int vertexShader;
        vertexShader = f->glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        const char *vertex = vertex_source[i].c_str();
        f->glShaderSource(vertexShader, 1, &vertex, NULL);
        f->glCompileShader(vertexShader);
        // check if successfully compiled
        check_vertex_shader(vertexShader);

        shader_program[i] = f->glCreateProgram();
        f->glAttachShader(shader_program[i], vertexShader);
        f->glAttachShader(shader_program[i], fragmentShader);
        f->glLinkProgram(shader_program[i]);
        check_shader_program(shader_program[i]);
        shaders_compiled[i] = true;
        f->glUseProgram(shader_program[i]);
        f->glDeleteShader(vertexShader);
    }
    f->glDeleteShader(fragmentShader);
}

template <>
class Shader<RENDER_TYPE::CUSTOM> {
  protected:
    unsigned shader_program; /**< shader program array depending on
                                which vertex size is chosen*/
    bool shaders_compiled = false;
    ; /**< true/false depending on whether the shaders were
         successfully compiled*/

    std::string vertex_source;
    std::string fragment_source;
    std::string geometry_source;

    QOpenGLFunctions *f;
    /**
     * @brief Constructor compiles shaders and tests them.
     */
    void compile_shaders() {
        // FRAGMENT SHADER

        unsigned int fragmentShader;
        fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
        // load fragment shader and compile it
        const char *frag = fragment_source.c_str();
        f->glShaderSource(fragmentShader, 1, &frag, NULL);
        f->glCompileShader(fragmentShader);
        this->check_fragment_shader(fragmentShader);

        //////////////////////////////////////////////////
        // VERTEX SHADER

        unsigned int vertexShader;
        vertexShader = f->glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        // const char *source =
        // shaders::uniform_vertex_shaders[i].c_str(); std::cout <<
        // source << std::endl;
        const char *vertex = vertex_source.c_str();
        f->glShaderSource(vertexShader, 1, &vertex, NULL);
        f->glCompileShader(vertexShader);
        this->check_vertex_shader(vertexShader);

        //////////////////////////////////////////////////
        // GEOMETRY SHADER

        bool geometry_shader_provided = true;
        unsigned int geometryShader;
        if (geometry_source.compare("") == 0) {
            geometry_shader_provided = false;
        } else {
            geometryShader = f->glCreateShader(GL_GEOMETRY_SHADER);
            const char *geometry = geometry_source.c_str();
            f->glShaderSource(geometryShader, 1, &geometry, NULL);
            f->glCompileShader(geometryShader);
            this->check_geometry_shader(geometryShader);
        }
        ///////////////////////////////////////
        // CREATE SHADER PROGRAM

        shader_program = f->glCreateProgram();
        f->glAttachShader(shader_program, vertexShader);
        if (geometry_shader_provided)
            f->glAttachShader(shader_program, geometryShader);
        f->glAttachShader(shader_program, fragmentShader);
        f->glLinkProgram(shader_program);
        this->check_shader_program(shader_program);
        shaders_compiled = true;
        // glUseProgram(shaderProgram);
        f->glDeleteShader(vertexShader);
        if (geometry_shader_provided)
            f->glDeleteShader(geometryShader);
        f->glDeleteShader(fragmentShader);
        std::cout << "shader successfully compiled!" << std::endl;
    }

    /**
     * @brief check if vertex shader has been successfully compiled
     * @param shader_ an unsigned integer referring to vertex shader
     */
    bool check_vertex_shader(const unsigned shader_) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];
        f->glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

        if (!success) {
            f->glGetShaderInfoLog(shader_, 512, NULL, infoLog);
            std::cout
                << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief check if fragment shader has been successfully
     * compiled
     * @param shader_ an unsigned integer referring to fragment
     * shader
     */
    bool check_fragment_shader(const unsigned shader_) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];
        f->glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

        if (!success) {
            f->glGetShaderInfoLog(shader_, 512, NULL, infoLog);
            std::cout
                << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief check if fragment shader has been successfully
     * compiled
     * @param shader_ an unsigned integer referring to fragment
     * shader
     */
    bool check_geometry_shader(const unsigned shader_) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];
        f->glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

        if (!success) {
            f->glGetShaderInfoLog(shader_, 512, NULL, infoLog);
            std::cout
                << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief check if shader program has been successfully linked
     * @param shaderProgram an unsigned integer referring to shader
     * program
     */
    bool check_shader_program(const unsigned shaderProgram) {
        // check if shader successfully compiled
        int success;
        char infoLog[512];

        f->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            f->glGetProgramInfoLog(shaderProgram, 512, NULL,
                                   infoLog);
            std::cout
                << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            return false;
        }
        return true;
    }

  public:
    Shader(QOpenGLFunctions *f_, const std::string &vertex_source_,
           const std::string &geometry_source_,
           const std::string &fragment_source_)
        : f(f_), vertex_source(vertex_source_),
          geometry_source(geometry_source_),
          fragment_source(fragment_source_) {
        compile_shaders();
    }

    void reinitialize_shader_program(std::string vertex_source_,
                                     std::string geometry_source_,
                                     std::string fragment_source_) {
        if (shaders_compiled)
            f->glDeleteProgram(shader_program);

        vertex_source = vertex_source_;
        fragment_source = fragment_source_;
        geometry_source = geometry_source_;
        compile_shaders();
    };

    void set_opengl_function_pointer(QOpenGLFunctions *f_) {
        f = f_;
    };

    void delete_program() { f->glDeleteProgram(shader_program); }

    Shader(){};
    Shader(Shader &&) = delete;
    Shader &operator=(Shader &&) = delete;
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    unsigned get_shader_program() { return shader_program; }
};
