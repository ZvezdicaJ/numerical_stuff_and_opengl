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
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 4;
    this->generate_vertexes();
    this->initialize_buffers();
    this->generate_filling_ebo();
};

template <typename T>
void Rectangle<T>::generate_vertexes() {
    this->vertexes = std::vector<T>({-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5});
}

template <typename T>
T Rectangle<T>::perimeter() {
    return 4;
}

#define __RECTENGLE__
#endif
