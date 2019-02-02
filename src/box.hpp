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
template <typename T>
Box<T>::Box() {
    this->vertex_size = 3;
    this->min_vertexes = 8;
    this->generate_vertexes();
    this->initialize_buffers();
};

/**
 * @brief This function sets the correct vertexes.
 */
template <typename T>
void Box<T>::generate_vertexes() {
    this->vertexes = aligned_vector<T>({-0.5, -0.5,
                                        -0.5, // a
                                        -0.5, 0.5,
                                        -0.5, // d
                                        -0.5, -0.5,
                                        0.5, // e
                                        0.5,  -0.5,
                                        -0.5, // b
                                        -0.5, 0.5,
                                        0.5, // h
                                        0.5,  0.5,
                                        -0.5, // c
                                        0.5,  -0.5,
                                        0.5,             // f
                                        0.5,  0.5,  0.5} // g
    );

    this->element_array =
        aligned_vector<int>({0,3,6,
                             6,2,0,
                             3,5,6,
                             5,7,6,
                             5,1,7,
                             7,4,1,
                             0,1,2,
                             1,2,4,
                             0,5,1,
                             0,5,3,
                             2,7,6,
                             2,7,4 });
}

#define __RECTENGLE__
#endif
