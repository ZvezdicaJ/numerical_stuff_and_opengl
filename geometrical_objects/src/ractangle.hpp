#ifndef __RECTANGLE__
template <typename T = float>
class Rectangle : public Shape2D<T> {
  private:
    void generate_vertexes();

  public:
    Rectangle();
    Rectangle(Rectangle &&) = default;
    Rectangle &operator=(Rectangle &&) = default;
    Rectangle(const Rectangle &) = default;
    Rectangle &operator=(const Rectangle &) = default;
    T perimeter();
};

template <typename T>
Rectangle<T>::Rectangle() {
    this->vertex_size = 2;
    this->min_vertexes = 4;
    this->generate_vertexes();
    this->initialize_buffers();
};

template <typename T>
void Rectangle<T>::generate_vertexes() {
    vertexes = std::vector<T>({-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5});
    element_array = std::vector<int>({0, 1, 2, 3, 0});
}

template <typename T>
T Rectangle<T>::perimeter() {
    return 4;
}

#define __RECTENGLE__
#endif
