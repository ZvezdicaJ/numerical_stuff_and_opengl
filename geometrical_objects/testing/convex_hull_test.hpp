TEST(convex_hull, point_side_2d) {

    std::array<float, 2> vec({0.7, 0.9});
    std::array<float, 2> point({0.2, -0.4});
    std::array<float, 2> vec_same({0.7, 0.9});

    ASSERT_EQ(point_side_2d(vec, point), SIDE::RIGHT);
    ASSERT_EQ(point_side_2d(point, vec), SIDE::LEFT);
    ASSERT_EQ(point_side_2d(vec_same, vec), SIDE::COLINEAR);
}

TEST(convex_hull, array_subtraction_operator_overload) {
    std::array<float, 4> a({1, 2, 3, 4});
    std::array<float, 4> b({5, -2, -4, -8});
    std::array<float, 4> result({-4, 4, 7, 12});
    ASSERT_EQ(result, a - b);

    std::array<float, 4> a2({10, 22, 35, 40});
    std::array<float, 4> b2({5, -2, 4, 8});
    std::array<float, 4> result2({5, 24, 31, 32});
    ASSERT_EQ(result2, a2 - b2);
}

TEST(convex_hull, convex_hull) {
    std::vector<std::array<float, 2>> points({{0.2, 0.9},
                                              {0.2, 0.5},
                                              {-0.5, -0.3},
                                              {-0.6, -0.9},
                                              {1.0, 1.0},
                                              {-1.0, 1.0},
                                              {1.0, -1.0},
                                              {-1.0, -1.0},
                                              {-0.3, 0.4}});
    std::list<std::array<float, 2>> solution(
        {{1.0, 1.0}, {-1.0, 1.0}, {1.0, -1.0}, {-1.0, -1.0}});
    convex_hull(points);
    // ASSERT_EQ(point_side_2d(point, vec), solution);
}
