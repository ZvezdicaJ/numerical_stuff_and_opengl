#ifndef __BOX__

/**
 *@class Box
 *@brief 3D box shape.
 */
template <typename T>
class Box : public Shape3D<T> {
  private:
    void generate_vertexes();

  public:
    Box();
    Box(Box &&) = default;
    Box &operator=(Box &&) = default;
    Box(const Box &) = default;
    Box &operator=(const Box &) = default;
    float perimeter();
};

/**
   @brief A basic constructor.
   @details The constructor generates vertexes and initializes buffers
 */
template <RENDER_TYPE T>
Box<T>::Box() {
    this->vertex_size = 4;
    this->min_vertexes = 8;
    this->generate_vertexes();
    this->initialize_buffers();
};

/**
 * @brief This function sets the correct vertexes.
 */
template <RENDER_TYPE T>
void Box<T>::generate_vertexes() {
    vertexes = std::vector<float>({-0.5,
                                   -0.5,
                                   -0.5, // a
                                   -0.5,
                                   0.5,
                                   -0.5, // d
                                   -0.5,
                                   -0.5,
                                   0.5, // e
                                   0.5,
                                   -0.5,
                                   -0.5, // b
                                   -0.5,
                                   0.5,
                                   0.5 // h
                                   0.5,
                                   0.5,
                                   -0.5 // c
                                   0.5,
                                   -0.5,
                                   0.5, // f
                                   0.5,
                                   0.5,
                                   0.5} // g
    );

    element_array = std::vector<int>({0, 2, 0, 3, 0, 1, 3, 5, 3, 6, 5, 7,
                                      5, 1, 4, 7, 2, 4, 1, 4, 2, 6, 6, 7});
}

#define __RECTENGLE__
#endif
