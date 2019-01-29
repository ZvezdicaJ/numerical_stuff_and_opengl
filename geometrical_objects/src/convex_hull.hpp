enum class SIDE { LEFT = -1, RIGHT = 1, COLINEAR = 0 };

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
template <typename T> SIDE point_side_2d(float *vec, float *point) {
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

template <typename T> std::vector<T> conex_hull(std::vector<T> collection) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");
    std::vector<T> convex_hull;
}
