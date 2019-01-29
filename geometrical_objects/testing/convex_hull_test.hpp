TEST(convex_hull, point_side_2d) {

    std::array<float, 2> vec({0.7, 0.9});
    std::array<float, 2> point({0.2, -0.4});

    ASSERT_EQ(point_side_2d(vec, point), SIDE::RIGHT);
    ASSERT_EQ(point_side_2d(point, vec), SIDE::LEFT);
}

TEST(convex_hull, convex_hull) {
    std::vector<std::array<float, 2>> vec({{0.2, 0.9},
                                           {0.2, 0.5},
                                           {-0.5, -0.3},
                                           {-0.6, -0.9},
                                           {1.0, 1.0},
                                           {-1.0, 1.0},
                                           {1.0, -1.0},
                                           {-1.0, -1.0}});
    std::list<std::array<float, 2>> solution(
        {{1.0, 1.0}, {-1.0, 1.0}, {1.0, -1.0}, {-1.0, -1.0}});

    ASSERT_EQ(point_side_2d(point, vec));
}
