#ifndef __ALIGNED_VECTOR__

/*template <typename T>
using aligned_vector =
    std::vector<T, boost::alignment::aligned_allocator<T, sizeof(T)>>;
*/
// it's better to just use 32 byte alignment because in case of large elements a lot of space might be sued - like in case of aligned_vector<__m256d>
using aligned_vector =
    std::vector<T, boost::alignment::aligned_allocator<T, 32>>;
#define __ALIGNED_VECTOR__ 1
#endif
