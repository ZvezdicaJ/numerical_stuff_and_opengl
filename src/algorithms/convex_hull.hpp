#pragma once
enum class SIDE { LEFT = -1, RIGHT = 1, COLINEAR = 0 };
/*
template <typename T, long unsigned N>
inline std::array<T, N> operator-(const std::array<T, N> &a,
                                  const std::array<T, N> &b) {
    std::array<T, N> result;
    for (long unsigned i = 0; i < N; i++) {
        result[i] = a[i] - b[i];
    }
    return result;
    }*/

template <typename T, long unsigned N>
inline std::array<T, N> operator-(const std::array<T, N> a,
                                  const std::array<T, N> b) {
    std::array<T, N> result;
    for (long unsigned i = 0; i < N; i++) {
        result[i] = a[i] - b[i];
    }
    return result;
}

/**
 * @brief The function return LEFT or RIGHT depending on which side of the
 * vector r the point lies
 * @param vec  vector relative to which the side is calculated
 * @param point point for which we calculate the side.
 */
template <typename T>
SIDE point_side_2d(std::array<T, 2> vec, std::array<T, 2> point) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");
    T z_comp = vec[0] * point[1] - point[0] * vec[1];
    if (fabs(z_comp) < 1e-8)
        return SIDE::COLINEAR;
    else if (z_comp > 0)
        return SIDE::LEFT;
    return SIDE::RIGHT;
}

/**
 * @brief The function return LEFT or RIGHT depending on which side of the
 * vector r the point lies
 * @param vec  vector relative to which the side is calculated
 * @param point point for which we calculate the side.
 */
template <typename T>
SIDE point_side_2d(float *vec, float *point) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");
    T z_comp = vec[0] * point[1] - point[0] * vec[1];
    T tol;
    if (std::is_same<T, float>::value)
        tol = 1e-6;
    else
        tol = 1e-12;

    if (fabs(z_comp) < tol)
        return SIDE::COLINEAR;
    else if (z_comp > 0)
        return SIDE::LEFT;
    return SIDE::RIGHT;
}

template <typename T>
std::vector<std::array<T, 2>>
convex_hull(std::vector<std::array<T, 2>> &collection) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");

    // sort the points
    std::sort(
        collection.begin(), collection.end(),
        [](std::array<T, 2> a, std::array<T, 2> b) { return a[0] < b[0]; });
    // for (unsigned j = 0; j < collection.size(); j++)
    //    std::cout << collection[j][0] << " " << collection[j][1] << std::endl;
    int s = (int)collection.size();
    // reserve Lupper
    std::vector<std::array<T, 2>> Lupper;
    Lupper.reserve(s);
    // reserve Llower
    std::vector<std::array<T, 2>> Llower;
    Llower.reserve(s);
    // put p1 and p2 into L upper
    if (s != 0) {
        Lupper.push_back(collection[0]);
        Llower.push_back(collection[s - 1]);
    }
    if (s > 1) {
        Lupper.push_back(collection[1]);
        Llower.push_back(collection[s - 2]);
    }
    if (collection.size() <= 2)
        return Lupper;
    // std::cout << "upper" << std::endl;
    for (unsigned i = 2; i < collection.size(); i++) {
        std::array<T, 2> next_point = collection[i];
        std::array<T, 2> vec =
            Lupper[Lupper.size() - 1] - Lupper[Lupper.size() - 2];
        while (SIDE::RIGHT != point_side_2d(vec, next_point)) {
            Lupper.pop_back();
            if (Lupper.size() == 1)
                break;
            vec = Lupper[Lupper.size() - 1] - Lupper[Lupper.size() - 2];
        }
        Lupper.push_back(next_point);
    }
    // std::cout << "lower: " << std::endl;
    for (int i = s - 3; i >= 0; i--) {
        //  std::cout << "i: " << i << std::endl;
        std::array<T, 2> next_point = collection[i];
        std::array<T, 2> vec =
            Llower[Llower.size() - 1] - Llower[Llower.size() - 2];
        while (SIDE::RIGHT != point_side_2d(vec, next_point)) {
            Llower.pop_back();
            if (Llower.size() == 1)
                break;
            vec = Llower[Llower.size() - 1] - Llower[Llower.size() - 2];
        }
        Llower.push_back(next_point);
    }
    /*
    std::cout << "Lupper size: " << Lupper.size() << std::endl;
    for (int i = 0; i < Lupper.size(); i++) {
        std::cout << Lupper[i][0] << " " << Lupper[i][1] << std::endl;
    }
    std::cout << "Llower size: " << Llower.size() << std::endl;
    for (int i = 0; i < Llower.size(); i++) {
        std::cout << Llower[i][0] << " " << Llower[i][1] << std::endl;
    }*/

    int d = Lupper.size();
    Lupper.resize(Lupper.size() + Llower.size() - 2);
    apex::memcpy((void *)&Lupper[d], (void *)&Llower[1],
                 sizeof(std::array<T, 2>) * (Llower.size() - 2));
    // memcpy((void *)&Lupper[d], (void *)&Llower[1],
    //       sizeof(std::array<T, 2>) * (Llower.size() - 2));
    /*
    std::cout << "Lupper size: " << Lupper.size() << std::endl;
    for (int i = 0; i < Lupper.size(); i++) {
        std::cout << Lupper[i][0] << " " << Lupper[i][1] << std::endl;
        }*/
    return Lupper;
}
