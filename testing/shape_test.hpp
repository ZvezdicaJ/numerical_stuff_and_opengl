
TEST(shape, sphere) {
    Sphere<float> sphere1(20000);
    EXPECT_THAT(sphere1.area(), testing::FloatNear(4.0 * M_PI, 1e-2));
    // std::cout << "sphere area: " << sphere.area() << std::endl;
    Sphere<double> sphere2(50000);
    EXPECT_THAT(sphere2.area(), testing::DoubleNear(4.0 * M_PI, 1e-2));
}

TEST(shape, box) {
    Box<float> box1;
    EXPECT_THAT(box1.area(), testing::FloatNear(6.0, 1e-6));

    Box<double> box2;
    EXPECT_THAT(box2.area(), testing::DoubleNear(6.0, 1e-6));
    // std::cout << "sphere area: " << sphere.area() << std::endl;
}
