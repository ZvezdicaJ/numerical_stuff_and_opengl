#pragma once
#include "Ising_model.hpp"

template <typename T>
class SpinArray;

template <typename T>
void draw_frame(Shader<RENDER_TYPE::CUSTOM> &shader_object,
                SpinArray<T> &spin_array) {

    unsigned shaderProgram = shader_object.get_shader_program();
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0);
    trans = glm::translate(trans, spin_array.pos);
    /* trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    */
    trans = glm::scale(trans, spin_array.scale);

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
void draw_black_white(Shader<RENDER_TYPE::CUSTOM> &shader_object,
                      SpinArray<T> &spin_array, IsingModel<T> &ising_model) {

    unsigned size = spin_array.vertexes.size() / spin_array.vertex_size;

    unsigned shaderProgram = shader_object.get_shader_program();
    glUseProgram(shaderProgram);

    glm::mat4 trans = glm::mat4(1.0);
    trans = glm::translate(trans, spin_array.pos);
    /* trans = glm::rotate(
        trans, (float)angle,
        glm::vec3(rotation_axis[0], rotation_axis[1], rotation_axis[2]));
    */
    trans = glm::scale(trans, spin_array.scale);

    unsigned int transformLoc =
        glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    // std::cout << glm::to_string(trans) << std::endl;

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

    glVertexAttribPointer(0, spin_array.vertex_size, type, GL_FALSE,
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
    glm::vec3 pos = {0.0, 0.0, 0.0};
    glm::vec3 pos_before_moved = {0.0, 0.0, 0.0};
    glm::vec3 scale = {1.0, 1.0, 1.0};
    std::array<T, 4> square;
    bool move = false;

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
        T initial_pos = -0.5 + square_size / 2.0; // step_size / 2.0;

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
    SpinArray(unsigned size_ = 50, glm::vec3 pos_ = {0.0, 0.0, 0.0})
        : size(size_), square_size((T)1.0 / (T)size_) {
        generate_vertexes();
        initialize_buffers();
        set_scale();
        set_pos(pos_);
    }

    void set_clickable_square(GLFWwindow *window) {

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        square = std::array<T, 4>{
            width / 2 - width / 4 * scale[0] + pos[0] * width / 2,
            width / 2 + width / 4 * scale[0] + pos[0] * width / 2,
            height / 2 - height / 4 * scale[1] + pos[1] * height / 2,
            height / 2 + height / 4 * scale[1] + pos[1] * height / 2,
        };
        std::cout << "pos: " << pos[0] << "  " << pos[1] << std::endl;
        std::cout << "clickable square at the start: " << square[0] << "  "
                  << square[1] << "  " << square[2] << "  " << square[3]
                  << std::endl;
    }

    void set_scale(glm::vec3 scale_ = {1, 1, 1}) { scale = scale_; };

    void set_pos(glm::vec3 pos_ = {0, 0, 0}) {
        pos = pos_;
        pos_before_moved = pos_;
    };

    unsigned get_size() { return size; }
    aligned_vector<T> get_vertexes() { return vertexes; }
    T get_square_size() { return square_size; }

    void movable(GLFWwindow *window) {

        static_assert(
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value &&
                "move_square function is defined only for floats and doubles");

        T min_x = square[0];
        T max_x = square[1];
        T min_y = square[2];
        T max_y = square[3];

        std::array<double, 2> cursor_pos;

        if (move == false &&
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwGetCursorPos(window, cursor_pos.data(), cursor_pos.data() + 1);

            T pos_x = (T)cursor_pos[0];
            T pos_y = (T)cursor_pos[1];

            if (pos_x > min_x && pos_x < max_x && pos_y > min_y &&
                pos_y < max_y) {
                move = true;
                std::cout << "move: " << move << std::endl;
            }

        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) !=
                   GLFW_PRESS) {
            if (move == true) {
                move = false;
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                square[0] +=
                    (float)(pos[0] - pos_before_moved[0]) / 2.0 * width;
                square[1] +=
                    (float)(pos[0] - pos_before_moved[0]) / 2.0 * width;
                square[2] -=
                    (float)(pos[1] - pos_before_moved[1]) / 2.0 * height;
                square[3] -=
                    (float)(pos[1] - pos_before_moved[1]) / 2.0 * height;
                std::cout << "square updated:" << std::endl;
                std::cout << "min_x: " << square[0] << "min_y: " << square[2]
                          << std::endl;

                pos_before_moved = pos;
            }
        }
        if (move == true) {
            std::array<double, 2> new_cursor_pos;
            // std::cout << "square clicked!" << std::endl;
            glfwGetCursorPos(window, new_cursor_pos.data(),
                             new_cursor_pos.data() + 1);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            pos[0] = pos_before_moved[0] +
                     (float)(new_cursor_pos[0] - cursor_pos[0]) / (float)width;

            pos[1] = pos_before_moved[1] -
                     (float)(new_cursor_pos[1] - cursor_pos[1]) / (float)height;
        }
    }

    // friends
    friend void draw_frame<T>(Shader<RENDER_TYPE::CUSTOM> &, SpinArray<T> &);

    friend void draw_black_white<T>(Shader<RENDER_TYPE::CUSTOM> &,
                                    SpinArray<T> &, IsingModel<T> &);
};
