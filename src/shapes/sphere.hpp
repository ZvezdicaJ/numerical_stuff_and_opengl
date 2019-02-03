#pragma once
#ifndef __SPHERE__

/** @class This class contains vertex and element data for 3d sphere.
 */
template <typename T = float>
class Sphere : public Shape3D<T> {
  private:
    void generate_vertexes_helper_old();
    void generate_vertexes_helper();
    void generate_vertexes();

  public:
    /** @brief A basic constructor for Sphere class.
     *  @details The constructor generate vertexes and elements. Opengl buffers
     * are also allocated and initiallized.
     */
    Sphere() {
        this->vertex_size = 3;
        this->min_vertexes = 50;
        generate_vertexes();
        this->initialize_buffers();
    };

    /** @brief A basic constructor for Sphere class.
     *  @details The constructor generate vertexes and elements. Opengl buffers
     * are also allocated and initiallized.
     *  @param min_vertexes_ minimal number of vertexes to create sphere
     */
    Sphere(int min_vertexes_) {
        this->vertex_size = 3;
        this->min_vertexes = min_vertexes_;
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

/** @brief This function generates vertexes for class Circle
 *  @details Internally, it calls generate_vertexes_helper function to refine
 * the mesh.
 */
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
    this->element_array = aligned_vector<int>(
        {0, 1, 3, 0, 1, 4, 0, 2, 3, 0, 2, 4, 1, 2, 3, 1, 2, 4});
    generate_vertexes_helper();
}

#ifdef __SSE__
/** @brief This function generates vertexes for float version of class Circle
 *  @details Internally, it uses sse instructions to generate mesh and set
 *  correct elements
 */
template <>
inline void Sphere<float>::generate_vertexes_helper() {
    int vertex_number = this->vertexes.size() / 3;
    this->vertexes.reserve(this->vertexes.size() * 5 + 1);
    int num_tri = this->element_array.size() / 3;
    aligned_vector<int> new_element_array; // = element_array;
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
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p23 = _mm_div_ps(
            _mm_add_ps(vert2, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));
        __m128 p13 = _mm_div_ps(
            _mm_add_ps(vert1, vert3),
            _mm_set_ps(std::numeric_limits<float>::infinity(), 2.0, 2.0, 2.0));

        float norm12 = std::sqrt(CalcDotProduct(p12, p12));
        float norm23 = std::sqrt(CalcDotProduct(p23, p23));
        float norm13 = std::sqrt(CalcDotProduct(p13, p13));

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
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 9],
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
    if (this->vertexes.size() < this->min_vertexes) {
        //std::cout << "recursive call to generate vertexes" << std::endl;
        generate_vertexes_helper();
    }

    // std::cout << "vertexes size: " << vertexes.size() << std::endl;
    // print_vertexes(&(this->vertexes[0]), this->vertexes.size() / 3);
    // std::cout << "\n\n" << element_array << std::endl;
}
#endif

#ifdef __AVX2__
/** @brief This function generates vertexes for float version of class Circle
 *  @details Internally, it uses avx2 instructions to generate mesh and set the
 *  correct elements
 */
template <>
inline void Sphere<double>::generate_vertexes_helper() {
    int vertex_number = this->vertexes.size() / 3;
    this->vertexes.reserve(this->vertexes.size() * 5 + 1);
    int num_tri = this->element_array.size() / 3;
    aligned_vector<int> new_element_array; // = element_array;
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

        __m256d vert1 = _mm256_loadu_pd(&(this->vertexes[vert1_ind * 3]));
        __m256d vert2 = _mm256_loadu_pd(&(this->vertexes[vert2_ind * 3]));
        __m256d vert3 = _mm256_loadu_pd(&(this->vertexes[vert3_ind * 3]));
        __m256d p12 =
            _mm256_div_pd(_mm256_add_pd(vert1, vert2),
                          _mm256_set_pd(std::numeric_limits<double>::infinity(),
                                        2.0, 2.0, 2.0));
        __m256d p23 =
            _mm256_div_pd(_mm256_add_pd(vert2, vert3),
                          _mm256_set_pd(std::numeric_limits<double>::infinity(),
                                        2.0, 2.0, 2.0));
        __m256d p13 =
            _mm256_div_pd(_mm256_add_pd(vert1, vert3),
                          _mm256_set_pd(std::numeric_limits<double>::infinity(),
                                        2.0, 2.0, 2.0));

        double norm12 = std::sqrt(CalcDotProduct(p12, p12));
        double norm23 = std::sqrt(CalcDotProduct(p23, p23));
        double norm13 = std::sqrt(CalcDotProduct(p13, p13));

        p12 = _mm256_div_pd(p12, _mm256_set_pd(1.0, norm12, norm12, norm12));
        p13 = _mm256_div_pd(p13, _mm256_set_pd(1.0, norm13, norm13, norm13));
        p23 = _mm256_div_pd(p23, _mm256_set_pd(1.0, norm23, norm23, norm23));

        int vertexes_size = this->vertexes.size();
        if (p12_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p12);
            p12_ind = vertex_number;
            new_vertex_indexing[ind_pair1] = p12_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p23_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p23);
            p23_ind = vertex_number;
            new_vertex_indexing[ind_pair2] = p23_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p13_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p13);
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
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size],
                         new_tri1);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 3],
                         new_tri2);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 6],
                         new_tri3);
        _mm_storeu_si128((__m128i *)&new_element_array[el_array_size + 9],
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
        generate_vertexes_helper();
    // print_vertexes(&vertexes[0], vertexes.size() / 3);
    // std::cout << "\n\n" << element_array << std::endl;
}
#endif

#ifndef __SSE__
/** @brief This function generates vertexes for float and double version of
 * class Circle.
 *  @details It is used only when sse and avx versions are not available.
 */
template <typename T>
inline void Sphere<T>::generate_vertexes_helper() {
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

        int v1 = (this->vertexes[vert1_ind * 3]);
        T *vert1 = &(this->vertexes[vert1_ind * 3]);
        T *vert2 = &(this->vertexes[vert2_ind * 3]);
        T *vert3 = &(this->vertexes[vert3_ind * 3]);

        T p12[3] = {(vert1[0] + vert2[0]) / 2.0, (vert1[1] + vert2[1]) / 2.0,
                    (vert1[2] + vert2[2]) / 2.0};

        T p23[3] = {(vert2[0] + vert3[0]) / 2.0, (vert2[1] + vert3[1]) / 2.0,
                    (vert2[2] + vert3[2]) / 2.0};

        T p13[3] = {(vert1[0] + vert3[0]) / 2.0, (vert1[1] + vert3[1]) / 2.0,
                    (vert1[2] + vert3[2]) / 2.0};

        T norm12 = std::sqrt(CalcDotProduct(p12, p12));
        T norm23 = std::sqrt(CalcDotProduct(p23, p23));
        T norm13 = std::sqrt(CalcDotProduct(p13, p13));

        for (int k = 0; k < 3; k++) {
            p12[k] = p12[k] / norm12;
            p13[k] = p13[k] / norm13;
            p23[k] = p23[k] / norm23;
        }

        int vertexes_size = this->vertexes.size();
        if (p12_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p12);
            p12_ind = vertex_number;
            new_vertex_indexing[ind_pair1] = p12_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p23_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p23);
            p23_ind = vertex_number;
            new_vertex_indexing[ind_pair2] = p23_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }
        if (p13_ind == -1) {
            this->vertexes.resize(this->vertexes.size() + 3);
            _mm256_storeu_pd(&(this->vertexes[vertexes_size]), p13);
            p13_ind = vertex_number;
            new_vertex_indexing[ind_pair3] = p13_ind;
            vertex_number += 1;
            vertexes_size += 3;
        }

        int el_array_size = new_element_array.size();
        new_element_array.resize(el_array_size + 12);
        new_element_array[el_array_size] = p12_ind;
        new_element_array[el_array_size + 1] = p23_ind;
        new_element_array[el_array_size + 2] = this->element_array[3 * tri + 1];
        new_element_array[el_array_size + 3] = p23_ind;
        new_element_array[el_array_size + 4] = p13_ind;
        new_element_array[el_array_size + 5] = this->element_array[3 * tri + 2];
        new_element_array[el_array_size + 6] = p12_ind;
        new_element_array[el_array_size + 7] = p13_ind;
        new_element_array[el_array_size + 8] = this->element_array[3 * tri];
        new_element_array[el_array_size + 9] = p12_ind;
        new_element_array[el_array_size + 10] = p23_ind;
        new_element_array[el_array_size + 11] = p13_ind;

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
        generate_vertexes_helper();
    // print_vertexes(&vertexes[0], vertexes.size() / 3);
    // std::cout << "\n\n" << element_array << std::endl;
}
#endif

/**
   @brief This function calls generate_vertexes_helper to refine the mesh.
 */
template <typename T>
void Sphere<T>::refine() {
    generate_vertexes_helper();
}

/**
   @brief The quality of the mesh is measured as the difference between exact pi
   number and the one obtained from the area of the sphere (mesh).
*/
template <typename T>
T Sphere<T>::quality() {
    return std::fabs(this->area() - M_PI);
}

#define __SPHERE__
#endif
