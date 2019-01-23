#ifndef __STAR3D__

/**
   @class Star3d
   @brief this class contains vertexes and elements for 3 dimensional star shape.
 */
template <typename T = float> class Star3d : public Shape3D<T> {
  private:
    void generate_vertexes(int = 10, T = 0.5);

  public:
    Star3d();
    Star3d(int, T = 0.5);
    Star3d(Star3d &&) = default;
    Star3d &operator=(Star3d &&) = default;
    Star3d(const Star3d &) = default;
    Star3d &operator=(const Star3d &) = default;
    T perimeter();
    T ratio;
};

template <typename T> Star3d<T>::Star3d() {
    this->draw_type = 'V';
    this->vertex_size = 3;
    this->min_vertexes = 2;
    this->generate_vertexes();
    this->initialize_buffers();
    this->generate_filling_ebo();
    // std::cout << "circle buffers:\nVBO: " << this->VBO << "\nVAO: " <<
    // this->VAO << std::endl;
};

template <typename T> Star3d<T>::Star3d(int bulges, T ratio_) : ratio(ratio_) {
    this->draw_type = 'V';
    this->vertex_size = 2;
    this->min_vertexes = 50;
    this->generate_vertexes(bulges, ratio);
    this->initialize_buffers();
};

/**
   @brief Generates vertexes for 3d star.
 */
template <typename T>
inline void Star3d<T>::generate_vertexes(int bulges, T ratio) {
    // this function always generates 4n-1 different vertexes;
    // -1 becase the last point is the same as the first one

    this->vertexes.reserve(2 * 3 *
                           bulges); // 2 because to each leg owns 2 points and 3
                                    // because each point has 3 coordinates
    this->vertexes.resize(2 * 3 *
                          bulges); // 2 because to each leg owns 2 points and 3
                                   // because each point has 3 coordinates
    int tocke = 2 * bulges;
    int r = 4 * ((tocke) / 4); // points should be a multiple of 4
    int reminder = tocke - r;  // stevilo preostalih tock

    // first number is number of steps in fi direction.
    // second number is number of steps in theta direction.
    std::pair<int, int> steps = closest_pair(find_products(bulges));

    float korak_fi = M_PI / steps.first;
    float korak_c_theta = 2.0 / steps.second;

    int index;

    for (int j = 0; j < steps.second; j++) {
        T ct = -1 + korak_c_theta;
        T st = sqrt(1 - ct * ct);
        for (int i = 0; i < steps.first; i++) {
            T fi = i * korak_fi;
            T cf = cos(fi);
            T sf = sin(fi);
            vertexes[index] = st * cf;
            vertexes[index + 1] = st * sf;
            vertexes[index + 2] = ct;
            index += 3;
        }
    }
}


#define __STAR3D__
#endif
