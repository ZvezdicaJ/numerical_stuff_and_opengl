#ifndef __SPHERE__

template <typename T = float>
class Sphere : public Shape3D<T> {
  private:
    void generate_vertexes_helper();
    void generate_vertexes_helper_improved();
    void generate_vertexes();

  public:
    Sphere()  {
        this->vertex_size = 3;
        generate_vertexes();
        this->initialize_buffers();
    };
    Sphere(Sphere &&) = default;
    Sphere &operator=(Sphere &&) = default;
    Sphere(const Sphere &) = default;
    Sphere &operator=(const Sphere &) = default;
    void refine();
    T quality();
    using Shape3D<T>::area;
};

template <typename T>
void Sphere<T>::generate_vertexes() {
    this->vertexes.reserve(15);
    this->vertexes.resize(15);
    std::array<T, 3> dir1({1.0, 0, 0});
    std::array<T, 3> dir2 = {std::cos(2.0 * M_PI / 3.0),
                             std::sin(2.0 * M_PI / 3.0), 0};
    std::array<T, 3> dir3 = {std::cos(4.0 * M_PI / 3.0),
                             std::sin(4.0 * M_PI / 3.0), 0};
    std::array<T, 3> z_dir = {0, 0, 1.0};
    // vertex 1
    this->vertexes[0] = dir1[0];
    this->vertexes[1] = dir1[1];
    this->vertexes[2] = dir1[2];
    // vertex 2
    this->vertexes[3] = dir2[0];
    this->vertexes[4] = dir2[1];
    this->vertexes[5] = dir2[2];
    // vertex 3
    this->vertexes[6] = dir3[0];
    this->vertexes[7] = dir3[1];
    this->vertexes[8] = dir3[2];
    // vertex 4
    this->vertexes[9] = z_dir[0];
    this->vertexes[10] = z_dir[1];
    this->vertexes[11] = z_dir[2];
    // vertex 5
    this->vertexes[12] = -z_dir[0];
    this->vertexes[13] = -z_dir[1];
    this->vertexes[14] = -z_dir[2];

    // definition of triangles we will later divide to create very fine mesh for
    // out sphere print_vertexes<float, 15>(vertexes);
    this->element_array = std::vector<int>(
        {0, 1, 3, 0, 1, 4, 0, 2, 3, 0, 2, 4, 1, 2, 3, 1, 2, 4});
    generate_vertexes_helper_improved();
}

template <typename T>
void Sphere<T>::generate_vertexes_helper_improved() {
    int vertex_number = this->vertexes.size() / 3;
    this->vertexes.reserve(this->vertexes.size() * 5 + 1);
    int num_tri = this->element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * this->element_array.size() + 1);
    std::unordered_map<std::pair<int, int>, int> new_vertex_indexing;
    new_vertex_indexing.reserve(vertex_number + 1);
    // print_vertexes(&vertexes[0], vertex_number);
    for (int tri = 0; tri < num_tri; tri++) {
        int vert1_ind = this->element_array[3 * tri];
        int vert2_ind = this->element_array[3 * tri + 1];
        int vert3_ind = this->element_array[3 * tri + 2];
        int p12_ind = -1;
        int p23_ind = -1;
        int p13_ind = -1;

        std::pair<int, int> ind_pair1;
        std::pair<int, int> ind_pair2;
        std::pair<int, int> ind_pair3;

        // setting up index pairs to be checked
        if (vert1_ind < vert2_ind)
            ind_pair1 = std::pair<int, int>(vert1_ind, vert2_ind);
        else
            ind_pair1 = std::pair<int, int>(vert2_ind, vert1_ind);

        if (vert2_ind < vert3_ind)
            ind_pair2 = std::pair<int, int>(vert2_ind, vert3_ind);
        else
            ind_pair2 = std::pair<int, int>(vert2_ind, vert3_ind);

        if (vert1_ind < vert3_ind)
            ind_pair3 = std::pair<int, int>(vert1_ind, vert3_ind);
        else
            ind_pair3 = std::pair<int, int>(vert3_ind, vert1_ind);

        // checking index pairs
        auto iter = new_vertex_indexing.find(ind_pair1);
        if (iter != new_vertex_indexing.end())
            p12_ind = iter->second;

        iter = new_vertex_indexing.find(ind_pair2);
        if (iter != new_vertex_indexing.end())
            p23_ind = iter->second;

        iter = new_vertex_indexing.find(ind_pair3);
        if (iter != new_vertex_indexing.end())
            p13_ind = iter->second;

        __m128 vert1 = _mm_loadu_ps(&(this->vertexes[vert1_ind * 3]));
        __m128 vert2 = _mm_loadu_ps(&(this->vertexes[vert2_ind * 3]));
        __m128 vert3 = _mm_loadu_ps(&(this->vertexes[vert3_ind * 3]));
        __m128 p12 = _mm_div_ps(
            _mm_add_ps(vert1, vert2),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(
            _mm_add_ps(vert2, vert3),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(
            _mm_add_ps(vert1, vert3),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));

        T norm12 = std::sqrt(CalcDotProductSse(p12, p12));
        T norm23 = std::sqrt(CalcDotProductSse(p23, p23));
        T norm13 = std::sqrt(CalcDotProductSse(p13, p13));

        p12 = _mm_div_ps(p12, _mm_set_ps(1.0, norm12, norm12, norm12));
        p13 = _mm_div_ps(p13, _mm_set_ps(1.0, norm13, norm13, norm13));
        p23 = _mm_div_ps(p23, _mm_set_ps(1.0, norm23, norm23, norm23));

        int vertexes_size = this->vertexes.size();
        if (p12_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm_storeu_ps(&(this->vertexes[vertexes_size]), p12);
            p12_ind = vertex_number;
            new_vertex_indexing[ind_pair1] = p12_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p23_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm_storeu_ps(&(this->vertexes[vertexes_size]), p23);
            p23_ind = vertex_number;
            new_vertex_indexing[ind_pair2] = p23_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p13_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm_storeu_ps(&(this->vertexes[vertexes_size]), p13);
            p13_ind = vertex_number;
            new_vertex_indexing[ind_pair3] = p13_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        // set the correct vertex element numbers
        __m128i new_tri1 = _mm_set_epi32(-1, p12_ind, p23_ind,
                                         this->element_array[3 * tri + 1]);
        __m128i new_tri2 = _mm_set_epi32(-1, p23_ind, p13_ind,
                                         this->element_array[3 * tri + 2]);
        __m128i new_tri3 =
            _mm_set_epi32(-1, p12_ind, p13_ind, this->element_array[3 * tri]);
        __m128i new_tri4 = _mm_set_epi32(-1, p12_ind, p23_ind, p13_ind);

        int el_array_size = new_element_array.size();
        new_element_array.resize(el_array_size + 12);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 9],
                         new_tri4);

        // vertex_number = *std::max_element(std::begin(new_element_array),
        //                                  std::end(new_element_array)) +
        //                1;
        // std::cout << "new_vertex_indexing:\n"
        //          << new_vertex_indexing << std::endl;
        // std::cout << "new element array:  \n" << new_element_array <<
        // std::endl; print_vertexes(&vertexes[0], vertex_number);
    }
    this->element_array = new_element_array;
    if (this->vertexes.size() < this->min_vertexes)
        generate_vertexes_helper_improved();
    // print_vertexes(&vertexes[0], vertexes.size() / 3);
    // std::cout << "\n\n" << element_array << std::endl;
}

template <typename T>
void Sphere<T>::generate_vertexes_helper() {
    int vertex_number = this->vertexes.size() / 3;
    this->vertexes.reserve(this->vertexes.size() * 5 + 1);
    int num_tri = this->element_array.size() / 3;
    std::vector<int> new_element_array; // = element_array;
    new_element_array.reserve(4 * this->element_array.size() + 1);

    for (int tri = 0; tri < num_tri; tri++) {
        __m128 vert1 =
            _mm_loadu_ps(&(this->vertexes[this->element_array[3 * tri] * 3]));
        __m128 vert2 = _mm_loadu_ps(
            &(this->vertexes[this->element_array[3 * tri + 1] * 3]));
        __m128 vert3 = _mm_loadu_ps(
            &(this->vertexes[this->element_array[3 * tri + 2] * 3]));
        __m128 p12 = _mm_div_ps(
            _mm_add_ps(vert1, vert2),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(
            _mm_add_ps(vert2, vert3),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(
            _mm_add_ps(vert1, vert3),
            _mm_set_ps(std::numeric_limits<T>::infinity(), 2.0, 2.0, 2.0));

        T norm12 = std::sqrt(CalcDotProductSse(p12, p12));
        T norm23 = std::sqrt(CalcDotProductSse(p23, p23));
        T norm13 = std::sqrt(CalcDotProductSse(p13, p13));
        // std::cout << "norm12: " << norm12 << "  norm13:  " << norm13
        //         << "  norm23  :" << norm23 << std::endl;
        p12 = _mm_div_ps(p12, _mm_set_ps(1.0, norm12, norm12, norm12));
        p13 = _mm_div_ps(p13, _mm_set_ps(1.0, norm13, norm13, norm13));
        p23 = _mm_div_ps(p23, _mm_set_ps(1.0, norm23, norm23, norm23));

        T norm = CalcDotProductSse(p12, p12);
        int vertexes_size = this->vertexes.size();
        this->vertexes.resize(this->vertexes.size() + 9);

        _mm_storeu_ps(&(this->vertexes[vertexes_size], p12));
        _mm_storeu_ps(&(this->vertexes[vertexes_size + 3], p23));
        _mm_storeu_ps(&(this->vertexes[vertexes_size + 6], p13));
        // set the correct vertex element numbers
        __m128i new_tri1 = _mm_set_epi32(-1, vertex_number, vertex_number + 1,
                                         this->element_array[3 * tri + 1]);

        __m128i new_tri2 =
            _mm_set_epi32(-1, vertex_number + 1, vertex_number + 2,
                          this->element_array[3 * tri + 2]);
        __m128i new_tri3 = _mm_set_epi32(-1, vertex_number, vertex_number + 2,
                                         this->element_array[3 * tri]);
        __m128i new_tri4 = _mm_set_epi32(-1, vertex_number, vertex_number + 1,
                                         vertex_number + 2);

        int el_array_size = new_element_array.size();
        new_element_array.resize(el_array_size + 12);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i_u *)&new_element_array[el_array_size + 9],
                         new_tri4);

        vertex_number = *std::max_element(std::begin(new_element_array),
                                          std::end(new_element_array)) +
                        1;
    }
    this->element_array = new_element_array;
    if (this->vertexes.size() < this->min_vertexes)
        generate_vertexes_helper();
}

template <typename T>
void Sphere<T>::refine() {
    generate_vertexes_helper_improved();
}

template <typename T>
T Sphere<T>::quality() {
    return std::fabs(this->area() - M_PI);
}

#define __SPHERE__
#endif
