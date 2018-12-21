void display();

void pause();
void OnPlusPressed(GLFWwindow *window);
void OnMinusPressed(GLFWwindow *window);
void OnEnterPressed(GLFWwindow *window, unsigned &enter_count);
void OnClosePressed(GLFWwindow *window);
void initialize_shapes(std::vector<Shape<float> *> &);
void display(std::vector<Shape<float> *> &);

template <typename U>
void initialize_shapes(std::vector<Shape<U> *> &shapes) {
    static_assert(std::is_same<float, U>::value ||
                      std::is_same<double, U>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    shapes.emplace_back(new (Sphere<U>));
}

template <typename U>
void display(std::vector<Shape<U> *> &shapes) {
    static_assert(std::is_same<float, U>::value ||
                      std::is_same<double, U>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    Shader<RENDER_TYPE::UNIFORM_COLOR> shader;
    rotate_all_shapes(shapes, shader);
}

template <RENDER_TYPE T, typename U>
void display_all_shapes(std::vector<Shape<U> *> &shapes, Shader<T> &shader) {
    static_assert(std::is_same<float, U>::value ||
                      std::is_same<double, U>::value,
                  "Shapes can only be instantiated with floating point types: "
                  "float, double, long double!");
    for (Shape<U> *shape : shapes) {
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, {0.5, 0.5, 0.5}, 0,
             {0.2, 0.4, 0.5, 0.7});
    }
}

template <RENDER_TYPE T, typename U>
void rotate_all_shapes(std::vector<Shape<U> *> &shapes, Shader<T> &shader) {
    std::array<float, 3> axis({1, 1, 1});
    static float fi = 0.0;
    for (Shape<U> *shape : shapes) {
        fi += 0.05;
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, axis, fi,
             {0.2, 0.4, 0.5, 0.7});
    }
}
