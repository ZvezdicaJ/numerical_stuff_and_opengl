#ifndef __ALIGNED_VECTOR__

template <typename T>
using aligned_vector =
    std::vector<T, boost::alignment::aligned_allocator<T, sizeof(T)>>;

#define __ALIGNED_VECTOR__ 1
#endif
