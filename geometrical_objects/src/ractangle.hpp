#ifndef __RECTANGLE__
template <RENDER_TYPE T = RENDER_TYPE::UNIFORM_COLOR>
class Rectangle : public Shape2D<T> {
  private:
    void generate_vertexes();

  public:
    Rectengle();
    Rectengle(Rectengle &&) = default;
    Rectengle &operator=(Rectengle &&) = default;
    Rectengle(const Rectengle &) = default;
    Rectengle &operator=(const Rectengle &) = default;
    float perimeter();
};

template <RENDER_TYPE T> Rectengle<T>::Rectengle() {
    this->vertex_size = 2;
    this->min_vertexes = 4;
    this->generate_vertexes();
    this->compile_shaders();
    this->initialize_buffers();
};

template <RENDER_TYPE T> void Rectangle<T>::generate_vertexes() {
    vertexes = std::vector<float>({-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5});
    element_array = std::vector<int>({0, 1, 2, 3, 0});
}

#define __RECTENGLE__
#endif
