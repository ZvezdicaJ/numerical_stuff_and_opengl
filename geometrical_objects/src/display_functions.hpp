void display();

void pause();
void OnPlusPressed(GLFWwindow *window);
void OnMinusPressed(GLFWwindow *window);
void OnEnterPressed(GLFWwindow *window, unsigned &enter_count);
void OnClosePressed(GLFWwindow *window);
void initialize_shapes(std::vector<Shape<float> *> &);
void display(std::vector<Shape<float> *> &);

template <RENDER_TYPE T>
void display_all_shapes(std::vector<Shape<float> *> &shapes,
                        Shader<T> &shader) {
    for (Shape<float> *shape : shapes) {
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, {0.5, 0.5, 0.5}, 0,
             {0.2, 0.4, 0.5, 0.7});
    }
}

template <RENDER_TYPE T>
void rotate_all_shapes(std::vector<Shape<float> *> &shapes, Shader<T> &shader) {
    std::array<float, 3> axis({1, 1, 1});
    static float fi = 0.0;
    for (Shape<float> *shape : shapes) {
        fi += 0.05;
        draw(*shape, shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0}, axis, fi,
             {0.2, 0.4, 0.5, 0.7});
    }
}
