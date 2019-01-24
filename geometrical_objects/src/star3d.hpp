#ifndef __STAR3D__x

/**
   @class Star3d
   @brief this class contains vertexes and elements for 3 dimensional star
   shape.
 */
template <typename T = float>
class Star3d : public Shape3D<T> {
  private:
    void generate_vertexes(int = 10, T = 0.5);

  public:
    Star3d();
    Star3d(int = 5, T = 0.5);
    Star3d(Star3d &&) = default;
    Star3d &operator=(Star3d &&) = default;
    Star3d(const Star3d &) = default;
    Star3d &operator=(const Star3d &) = default;
    T perimeter();
    T ratio;
};

template <typename T>
Star3d<T>::Star3d(int bulges, T ratio_) : ratio(ratio_) {
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

    this->element_array.reserve(12 * bulges);
    this->element_array.resize(12 * bulges);
    // first number is number of steps in fi direction.
    // second number is number of steps in theta direction.
    std::pair<int, int> steps = closest_pair(find_products(bulges));

    float korak_fi = M_PI / steps.first;
    float korak_c_theta = 2.0 / steps.second;

    int index = 0;

    for (int j = 0; j <= steps.second; j++) {
        T ct = -1 + korak_c_theta;
        T st = sqrt(1 - ct * ct);

        T ct2 = -1 + korak_c_theta * 3.0 / 2.0;
        T st2 = sqrt(1 - ct2 * ct2);
        for (int i = 0; i < steps.first; i++) {
            T fi = i * korak_fi;
            T cf = cos(fi);
            T sf = sin(fi);
            vertexes[index] = st * cf;
            vertexes[index + 1] = st * sf;
            vertexes[index + 2] = ct;
            if (j != steps.second) {
                T fi2 = i * korak_fi + korak_fi / 2;
                T cf2 = cos(fi);
                T sf2 = sin(fi);
                vertexes[index + 3] = st2 * cf2;
                vertexes[index + 4] = st2 * sf2;
                vertexes[index + 5] = ct2;
                index += 6;
            } else
                index += 3;
        }
    }

    // set elements
    int column = 0;
    int row = 0;
    int mid_point_start = steps.first * steps.second;
    int triangles = 0;
    for (int i = 0; i < bulges; i++) {
        int mid_point = mid_point_start + (steps.first - 1) * row;
        int p1 = column + row * steps.first;
        int p2 = column + row * steps.first + 1;
        int p3 = column + (row + 1) * steps.first;
        int p4 = column + (row + 1) * steps.first + 1;
        element_array[triangle] =
    }
}

#define __STAR3D__
#endif
