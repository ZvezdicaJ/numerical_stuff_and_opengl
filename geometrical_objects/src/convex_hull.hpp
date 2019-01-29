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
std::list<std::array<T, 2>>
convex_hull(std::vector<std::array<T, 2>> &collection) {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "point_side_2d: point type should be float or double");

    // sort the points
    std::sort(
        collection.begin(), collection.end(),
        [](std::array<T, 2> a, std::array<T, 2> b) { return a[0] < b[0]; });
    for (unsigned j = 0; j < collection.size(); j++)
        std::cout << collection[j][0] << " " << collection[j][1] << std::endl;
    // reserve Lupper
    std::list<std::array<T, 2>> Lupper;
    // put p1 and p2 into L upper
    if (!collection.empty())
        Lupper.push_back(collection[0]);
    if (collection.size() > 1)
        Lupper.push_back(collection[1]);
    if (collection.size() <= 2)
        return Lupper;

    for (unsigned i = 2; i < collection.size(); i++) {
        Lupper.push_back(collection[i]);
        int d = Lupper.size();
        auto before_last = Lupper.end()--;
        before_last = before_last--;
        std::array<T, 2> vec = (*(before_last) - *(before_last--));
        while (Lupper.size() > 2 &&
               SIDE::RIGHT != point_side_2d(vec, *(Lupper.end()--))) {
            auto it = (Lupper.end()--);
            Lupper.erase(it--);
            break;
        }
    }
    /*
    // reserve Llower
    std::list<std::array<T, 2>> Llower;
    // put p1 and p2 into L upper
    Llower.push_back(collection[collection.size() - 1]);
    Llower.push_back(collection[collection.size() - 2]);
    if (collection.size() <= 2)
        return Llower;
    for (unsigned i = collection.size() - 2; i > 0; i++) {
        Llower.push_back(collection[i]);
        int d = Llower.size();
        auto before_last = Llower.end()--;
        before_last = before_last--;
        std::array<T, 2> vec = (*(before_last) - *(before_last--));
        while (Llower.size() > 2 &&
               SIDE::RIGHT != point_side_2d(vec, *Llower.end()--)) {
            Llower.erase((Llower.end()--)--);
        }
    }
    Llower.erase(Llower.end()--);
    Llower.erase(Llower.begin());
    Lupper.insert(Lupper.end(), Llower.begin(), Llower.end()--);
    return Lupper;
    */
}
