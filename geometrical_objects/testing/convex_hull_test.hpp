TEST(convex_hull, point_side_2d) {

    std::array<float, 2> vec({0.7, 0.9});
    std::array<float, 2> point({0.2, -0.4});

    EXPECT_THAT(point_side_2d(vec, point), SIDE::RIGHT);
    EXPECT_THAT(point_side_2d(point, vec), SIDE::LEFT);
}
