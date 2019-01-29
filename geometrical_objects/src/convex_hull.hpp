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
    T tol;
    if (std::is_same<T, float>)
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
std::vector<T> conex_hull(std::vector<std::array<T, 2>> collection) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");
    std::vector<T> convex_hull;
    // sort the points
    std::sort(collection.begin(), collection.end,
              [](auto a, auto b) { return a[0] > b[0] });
    // reserve Lupper
    std::list<std::array<T, 2>> Lupper;
    Lupper.reserve(collection.size());
    // put p1 and p2 into L upper
    Lupper.push_back(collection[0]);
    Lupper.push_back(collection[1]);
    if (collection.size() <= 2)
        return Lupper;
    for (int i = 2; i < collection.size(); i++) {
        Lupper.push_back(collection[i]);
        int d = Lupper.size();
        std::array<T, 2> vec({Lupper[d - 2][0] - Lupper[d - 3][0],
                              Lupper[d - 2][1] - Lupper[d - 3][1]});
        while (Lupper.size() > 2 &&
               SIDE::RIGHT != point_side_2d(vec, Lupper[d - 1])) {
            Lupper.erase(Lupper.end() - 2);
        }

        // reserve Lupper
        std::list<std::array<T, 2>> Lupper;
        Llower.reserve(collection.size());
        // put p1 and p2 into L upper
        Llower.push_back(collection[collection.size() - 1]);
        Llower.push_back(collection[collection.size() - 2]);
        if (collection.size() <= 2)
            return Llower;
        for (int i = collection.size() - 2; i > 0; i++) {
            Llower.push_back(collection[i]);
            int d = Llower.size();
            std::array<T, 2> vec({Llower[d - 2][0] - Llower[d - 3][0],
                                  Llower[d - 2][1] - Llower[d - 3][1]});
            while (Llower.size() > 2 &&
                   SIDE::RIGHT != point_side_2d(vec, Llower[d - 1])) {
                Llower.erase(Llower.end() - 2);
            }
        }
