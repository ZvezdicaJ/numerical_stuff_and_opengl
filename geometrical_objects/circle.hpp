
template <RENDER_TYPE T>
class Circle : public shape {
   public:
    circle();
  Circle(Circle &&) = default;
  Circle &operator=(Circle &&) = default;
  Circle(const Circle &) = default;
  Circle &operator=(const Circle &) = default;

    void generate_vertexes();
    float perimeter();
}

template <RENDER_TYPE T>x
void circle<T>::circle() : min_vertexes(10){};

template <RENDER_TYPE T>
void circle<T>::generate_vertexes() {


}
