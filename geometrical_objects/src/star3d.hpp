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
    Star3d(int = 6, T = 0.5);
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
    this->vertex_size = 3;
    this->min_vertexes = 4;
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

    // first number is number of steps in fi direction.
    // second number is number of steps in theta direction.
    std::pair<int, int> steps = closest_pair(find_products(bulges));

    float korak_fi = M_PI / steps.first;
    float korak_c_theta = 2.0 / steps.second;

    this->vertexes.reserve(3 * (2 * bulges + steps.first));
    // 2 because to each leg owns 2 points and
    // 3 because each point has 3 coordinates
    this->vertexes.resize(3 * (2 * bulges + steps.first));
    // 2 because to each leg owns 2 points and
    // 3 because each point has 3 coordinates

    this->element_array.reserve(12 * bulges);
    this->element_array.resize(12 * bulges);

    int index = 0;
    std::cout << "bulges: " << bulges << std::endl;
    std::cout << "fi steps: " << steps.first << std::endl;
    std::cout << "theta steps: " << steps.second << std::endl;
    std::cout << "element array size: " << 12 * bulges << std::endl;

    // loop v smeri theta - po vrsticah
    for (int j = 0; j <= steps.second; j++) {
        T ct = -1 + korak_c_theta;
        T st = sqrt(1 - ct * ct);

        T ct2 = -1 + korak_c_theta * 3.0 / 2.0;
        T st2 = sqrt(1 - ct2 * ct2);
        // loop v smeri phi - po stolpcih
        for (int i = 0; i < steps.first; i++) {
            T fi = i * korak_fi;
            T cf = cos(fi);
            T sf = sin(fi);
            this->vertexes[index] = ratio*st * cf;
            this->vertexes[index + 1] = ratio*st * sf;
            this->vertexes[index + 2] = ratio*ct;
            if (j != steps.second) {
                T fi2 = i * korak_fi + korak_fi / 2;
                T cf2 = cos(fi);
                T sf2 = sin(fi);
                this->vertexes[index + 3] = st2 * cf2;
                this->vertexes[index + 4] = st2 * sf2;
                this->vertexes[index + 5] = ct2;
                index += 6;
            } else
                index += 3;
        }
    }

    // variable for setting elements
    int column = 0;
    int row = 0;
    int triangles = 0;
    int element_index = 0;
    for (int j = 0; j < steps.second; j++) {
        // loop v smeri phi - po stolpcih
        for (int i = 0; i < steps.first; i++) {
            // the above code sets vertex data
            // below we set the correct elements

            int mid_point = j * 2 * steps.first + 2 * i + 1;
            int p1 = j * 2 * steps.first + 2 * i;
            int p3 = (j + 1) * 2 * steps.first + 2 * i;
            int p2, p4;
            if (i != steps.first - 1) {
                p2 = j * 2 * steps.first + 2 * i + 2;
                p4 = (j + 1) * 2 * steps.first + 2 * i + 2;
            } else {
                p2 = j * 2 * steps.first;
                p4 = (j + 1) * 2 * steps.first;
            }
            __m128i tri = _mm_set_epi32(p1, p2, mid_point, p2);
            _mm_stream_si128((__m128i *)&(this->element_array[element_index]),
                             tri);

            tri = _mm_set_epi32(p3, mid_point, p3, p4);
            _mm_stream_si128(
                (__m128i *)&(this->element_array[element_index + 4]), tri);

            tri = _mm_set_epi32(mid_point, p4, p1, mid_point);
            _mm_stream_si128(
                (__m128i *)&(this->element_array[element_index + 8]), tri);

            element_index += 12;

            std::cout << "tocke: " << p1 << "  " << p2 << "  " << mid_point
                      << "  " << p3 << "  " << p4 << std::endl;
        }
    }
    std::cout << "element index: " << element_index << std::endl;
    print_vertexes(&(this->vertexes[0]), (this->vertexes).size() / 3, 3);
}

#define __STAR3D__
#endif
