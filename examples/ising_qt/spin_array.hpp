#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <QWidget>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "shader_class_qt_version.hpp"
#include "Ising_model.hpp"
#include "ising_shader.hpp"

template <typename T>
class SpinArray : public QOpenGLWidget {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "SpinArray can only be instantiated with "
                  "floating point types: "
                  "float or double!");

  private:
    unsigned VBO, VAO, size,
        ssbo; // the last one is for shader buffer object - for
              // spin directions
    bool transparent = false;
    aligned_vector<T> vertexes;
    bool initialized = false;
    glm::vec3 pos = {0.0, 0.0, 0.0};
    glm::vec3 pos_before_moved = {0.0, 0.0, 0.0};
    glm::vec3 scale = {1.0, 1.0, 1.0};
    std::array<T, 4> square;
    bool move = false;
    Shader<RENDER_TYPE::CUSTOM> frame_shader;
    Shader<RENDER_TYPE::CUSTOM> array_shader;
    QOpenGLFunctions *f;
    QOpenGLExtraFunctions *fe;
    unsigned vertex_size = 2;
    T square_size;
    /**
     * @brief Allocates and initializes vertex buffer object,
     * element buffer object and vertex array object. It also
     * allocates color buffer - where color for each vertex is
     * stored.
     */
    void initialize_buffers() {

        fe->glGenVertexArrays(1, &VAO);
        fe->glBindVertexArray(VAO);

        // generate and bind and fill vertex data
        f->glGenBuffers(1, &VBO);
        f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
        f->glBufferData(GL_ARRAY_BUFFER,
                        sizeof(T) * vertexes.size(), &vertexes[0],
                        GL_DYNAMIC_DRAW);
        // the following is for shader buffer object, which is
        // loaded in drawing function
        f->glGenBuffers(1, &ssbo);
    }

    void generate_vertexes() {
        T step_size = 1.0 / (float)size;
        T initial_pos =
            -0.5 + square_size / 2.0; // step_size / 2.0;

        int index = 0;
        vertexes.reserve(size * size);

        for (int i = 0; i < size; i++) {
            // j goes over x coordinate
            for (int j = 0; j < size; j++) {
                vertexes.push_back(initial_pos + j * square_size);
                vertexes.push_back(initial_pos + i * square_size);
                index += 2;
            }
        }
    }

  public:
    SpinArray(QWidget *parent, unsigned size_ = 50,
              glm::vec3 pos_ = {0.0, 0.0, 0.0})
        : QOpenGLWidget(parent) {
        if (QOpenGLContext::currentContext() == 0)
            std::cerr << "parent has no context!!" << std::endl;
        square_size = (T)1.0 / (T)size_;
        size = size_;
        initializeGL();
        // generate_vertexes();
        // set_scale();
        // set_pos(pos_);
    }

    void initialize_shaders() {
        frame_shader.set_opengl_function_pointer(f);
        array_shader.set_opengl_function_pointer(f);

        frame_shader.reinitialize_shader_program(
            ising_frame_vertex_shader, ising_frame_geometry_shader,
            ising_frame_fragment_shader);

        array_shader.reinitialize_shader_program(
            ising_triangle_vertex_shader,
            ising_triangle_geometry_shader,
            ising_triangle_fragment_shader);
    }
    void cleanup() {

        makeCurrent();
        frame_shader.delete_program();
        array_shader.delete_program();
        f->glDeleteBuffers(1, &VBO);
        fe->glDeleteVertexArrays(1, &VAO);
        doneCurrent();
    }
    /** @brief This is an overload for internal initialize
     * function for opengl context
     */
    void initializeGL() {
        /* In this example the widget's corresponding top-level
           window can change several times during the widget's
           lifetime. Whenever this happens, the QOpenGLWidget's
           associated context is destroyed and a new one is
           created. Therefore we have to be prepared to clean up
           the resources on the aboutToBeDestroyed() signal,
           instead of the destructor. The emission of the signal
           will be followed by an invocation of initializeGL()
           where we can recreate all resources.
         */

        if (context() == 0)
            std::cerr << "context not initialized!" << std::endl;
        connect(context(), &QOpenGLContext::aboutToBeDestroyed,
                this, &SpinArray::cleanup);

        // initializeOpenGLFunctions();
        f = QOpenGLContext::currentContext()->functions();
        fe = QOpenGLContext::currentContext()->extraFunctions();

        glClearColor(0, 0, 0, transparent ? 0 : 1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        // initialize_shaders();
    }

    void set_scale(glm::vec3 scale_ = {1, 1, 1}) {
        scale = scale_;
    };

    void set_pos(glm::vec3 pos_ = {0, 0, 0}) {
        pos = pos_;
        pos_before_moved = pos_;
    };

    unsigned get_size() { return size; }
    aligned_vector<T> get_vertexes() { return vertexes; }
    T get_square_size() { return square_size; }

    void draw_frame() {

        unsigned shaderProgram = frame_shader.get_shader_program();
        f->glUseProgram(shaderProgram);

        glm::mat4 trans = glm::mat4(1.0);
        trans = glm::translate(trans, pos);
        /* trans = glm::rotate(
            trans, (float)angle,
            glm::vec3(rotation_axis[0], rotation_axis[1],
           rotation_axis[2]));
        */
        trans = glm::scale(trans, scale);

        unsigned int transformLoc =
            f->glGetUniformLocation(shaderProgram, "transform");
        f->glUniformMatrix4fv(transformLoc, 1, GL_FALSE,
                              glm::value_ptr(trans));

        unsigned int square_size_loc =
            f->glGetUniformLocation(shaderProgram, "square_size");
        f->glUniform1fv(square_size_loc, 1,
                        (GLfloat *)&square_size);

        GLenum type;
        if (std::is_same<double, T>::value)
            type = GL_DOUBLE;
        else
            type = GL_FLOAT;

        f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
        fe->glBindVertexArray(VAO);
        f->glVertexAttribPointer(0, vertex_size, type, GL_TRUE,
                                 vertex_size * sizeof(T),
                                 (void *)0);
        f->glEnableVertexAttribArray(0);

        f->glDrawArrays(GL_POINTS, 0,
                        vertexes.size() / vertex_size);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as
        // wireframe glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //
        // render as filled triangles
    };

    void draw_black_white(IsingModel<T> &ising_model) {

        unsigned size = vertexes.size() / vertex_size;

        unsigned shaderProgram = array_shader.get_shader_program();
        f->glUseProgram(shaderProgram);

        glm::mat4 trans = glm::mat4(1.0);
        trans = glm::translate(trans, pos);
        /* trans = glm::rotate(
            trans, (float)angle,
            glm::vec3(rotation_axis[0], rotation_axis[1],
           rotation_axis[2]));
        */
        trans = glm::scale(trans, scale);

        unsigned int transformLoc =
            f->glGetUniformLocation(shaderProgram, "transform");
        f->glUniformMatrix4fv(transformLoc, 1, GL_FALSE,
                              glm::value_ptr(trans));

        // std::cout << glm::to_string(trans) << std::endl;

        unsigned int square_size_loc =
            f->glGetUniformLocation(shaderProgram, "square_size");
        f->glUniform1fv(square_size_loc, 1,
                        (GLfloat *)&square_size);

        GLenum type;
        if (std::is_same<double, T>::value)
            type = GL_DOUBLE;
        else
            type = GL_FLOAT;

        f->glBindBuffer(GL_ARRAY_BUFFER, VBO);

        fe->glBindVertexArray(VAO);

        f->glVertexAttribPointer(0, vertex_size, type, GL_FALSE,
                                 vertex_size * sizeof(T),
                                 (void *)0);
        f->glEnableVertexAttribArray(0);

        f->glBindBuffer(GL_ARRAY_BUFFER, ising_model.VBO);
        f->glBufferData(GL_ARRAY_BUFFER, sizeof(int) * size,
                        (void *)ising_model.get_spin_array(),
                        GL_STATIC_DRAW);

        fe->glVertexAttribIPointer(1, 1, GL_INT, 0, (void *)0);
        f->glEnableVertexAttribArray(1);

        f->glDrawArrays(GL_POINTS, 0,
                        vertexes.size() / vertex_size);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as
        // wireframe glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //
        // render as filled triangles
    };
};
