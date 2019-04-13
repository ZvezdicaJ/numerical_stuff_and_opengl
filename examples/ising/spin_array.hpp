#pragma once
#include "Ising_model.hpp"

template <typename T>
class SpinArray;

template <typename T>
void draw_frame(GLFWwindow *window, Shader<RENDER_TYPE::CUSTOM> &shader_object,
                SpinArray<T> &spin_array,
                std::array<T, 3> position = {0.0, 0.0, 0.0},
                std::array<T, 3> scale = {1.0, 1.0, 1.0}) {

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    unsigned shaderProgram = shader_object.get_shader_program();
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                      static_cast<GLfloat>(height));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));

    glm::mat4 trans = glm::mat4(1.0);
    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    /* trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    */
    trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int square_size_loc =
        glGetUniformLocation(shaderProgram, "square_size");
    glUniform1fv(square_size_loc, 1, (GLfloat *)&spin_array.square_size);

    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, spin_array.VBO);
    glBindVertexArray(spin_array.VAO);
    glVertexAttribPointer(0, spin_array.vertex_size, type, GL_TRUE,
                          spin_array.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0,
                 spin_array.vertexes.size() / spin_array.vertex_size);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
};

template <typename T>
void draw_black_white(GLFWwindow *window,
                      Shader<RENDER_TYPE::CUSTOM> &shader_object,
                      SpinArray<T> &spin_array, IsingModel<T> &ising_model,
                      std::array<T, 3> position = {0.0, 0.0, 0.0},
                      std::array<T, 3> scale = {1.0, 1.0, 1.0}) {

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    unsigned size = spin_array.vertexes.size() / spin_array.vertex_size;

    unsigned shaderProgram = shader_object.get_shader_program();
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                      static_cast<GLfloat>(height));

    static int ind = 0;
    if (ind == 0) {
        std::cout << "window dims: " << width << "  " << height << std::endl;
        std::cout << glm::to_string(projection) << std::endl;
        ind = 1;
    }

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));

    glm::mat4 trans = glm::mat4(1.0);
    trans =
        glm::translate(trans, glm::vec3(position[0], position[1], position[2]));
    /* trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    */
    trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    unsigned int square_size_loc =
        glGetUniformLocation(shaderProgram, "square_size");
    glUniform1fv(square_size_loc, 1, (GLfloat *)&spin_array.square_size);
    /*
    static int ind = 0;

    if (ind == 0) {
        int *p = (int*)ising_model.get_spin_array();

        for (int u = 0; u < 6; u++) {
            for (int k = 0; k < 6; k++)
                std::cout << *(p + u * 6 + k) << " ";
            std::cout << " " << std::endl;
        }

        std::cout << " \n\n" << std::endl;
        for (int k = 0; k < size; k++)
            std::cout << " ( " << spin_array.vertexes[k] << ", "
                      << spin_array.vertexes[k + 1] << " ) " << std::endl;
        ind = 1;
    }
    */
    GLenum type;
    if (std::is_same<double, T>::value)
        type = GL_DOUBLE;
    else
        type = GL_FLOAT;

    glBindBuffer(GL_ARRAY_BUFFER, spin_array.VBO);

    glBindVertexArray(spin_array.VAO);

    glVertexAttribPointer(0, spin_array.vertex_size, type, GL_TRUE,
                          spin_array.vertex_size * sizeof(T), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, ising_model.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * size,
                 (void *)ising_model.get_spin_array(), GL_STATIC_DRAW);

    glVertexAttribIPointer(1, 1, GL_INT, 0, (void *)0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_POINTS, 0,
                 spin_array.vertexes.size() / spin_array.vertex_size);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // render as filled triangles
};

/** @class
 *
 *
 */
template <typename T>
class SpinArray {
    static_assert(
        std::is_same<float, T>::value || std::is_same<double, T>::value,
        "SpinArray can only be instantiated with floating point types: "
        "float or double!");

  private:
    unsigned VBO, VAO, size,
        ssbo; // the last one is for shader buffer object - for spin directions
    aligned_vector<T> vertexes;
    unsigned vertex_size = 2;
    T square_size;
    /**
     * @brief Allocates and initializes vertex buffer object, element buffer
     * object and vertex array object. It also allocates color buffer - where
     * color for each vertex is stored.
     */
    void initialize_buffers() {

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // generate and bind and fill vertex data
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertexes.size(), &vertexes[0],
                     GL_DYNAMIC_DRAW);
        // the following is for shader buffer object, which is loaded in drawing
        // function
        glGenBuffers(1, &ssbo);
    }

    void generate_vertexes() {
        T step_size = 1.0 / (float)size;
        T initial_pos = -0.5 + step_size / 2.0;

        int index = 0;
        vertexes.reserve(size * size);

        for (int i = 0; i < size; i++) {
            // j goes over x coordinate
            for (int j = 0; j < size; j++) {
                vertexes.push_back(initial_pos + j * step_size);
                vertexes.push_back(initial_pos + i * step_size);
                index += 2;
            }
        }
    }

  public:
    SpinArray(unsigned size_ = 50)
        : size(size_), square_size((T)1.0 / (T)size_) {
        generate_vertexes();
        initialize_buffers();
    }

    unsigned get_size() { return size; }
    aligned_vector<T> get_vertexes() { return vertexes; }
    T get_square_size() { return square_size; }

    friend void draw_frame<T>(GLFWwindow *, Shader<RENDER_TYPE::CUSTOM> &,
                              SpinArray<T> &, std::array<T, 3>,
                              std::array<T, 3>);

    friend void draw_black_white<T>(GLFWwindow *, Shader<RENDER_TYPE::CUSTOM> &,
                                    SpinArray<T> &, IsingModel<T> &,
                                    std::array<T, 3>, std::array<T, 3>);
};
