template <typename T>
class PoissonSOR {
    static_assert(std::is_same<float, T>::value ||
                      std::is_same<double, T>::value,
                  "Poisson SOR can only be instantiated with float "
                  "or double types ");
    unsigned size;
    T *p1;
    T *p2;
    T *result;

  public:
    PoissonSOR(unsigned size_, T *p1_, T *result_)
        : size(size_), p1(p1_), result(result_) {
        p2 = (T *)std::aligned_alloc(32, sizeof(T) * size * size);
    };

    void iteration();
    void solve();
};

template <>
inline void PoissonSOR<float>::iteration() {

    T *p3 = (T *)std::aligned_alloc(32, sizeof(T) * size * size);

    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            __m128 neighbours = _mm_i32gather_ps(p1+);
        }
    };
};
