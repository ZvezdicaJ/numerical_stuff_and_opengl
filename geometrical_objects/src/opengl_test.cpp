#include "opengl_test.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    print("Window created!");

    // glad takes care of opengl function pointers
    // you need this before you call any opengl functions
    // you have to do this after glfwMAkeContextCurrent(window)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    /*
    while (!glfwWindowShouldClose(window)) {
        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);

        std::array<float, 3> center({0, 0, 0});
        // draw_sphere(center, 0.5, fi);
        fi += 0.01;
        // sphere1.draw(0.2, {0, 0, 0}, {0, 1, 1}, fi, {0.2, 0.3, 0.4, 0.5});
        // sphere1.draw(0.1, {0.5 * std::cos(fi), 0.5 * std::sin(fi), 0},
        //             {0, 1, 1}, 2.0 * fi, {0.2, 0.3, 0.4, 0.5});
        // sphere1.set_min_number_of_vertexes(100);
        glfwPollEvents();  // check if any events happend (mouse press, key
                           // pres)

        // The glfwSwapBuffers will swap the color buffer (a large buffer that
        // contains color values for each pixel in GLFW’s window) that has been
        // used to draw in during this iteration and show it as output to the
        // screen.
        glfwSwapBuffers(window);

        glClearColor(0.2f, 0.3f, 0.3f,
                     1.0f);  // set which color to clear the screen with
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT.
        // set which buffer to use to clear the screen

        glClear(GL_COLOR_BUFFER_BIT);
        }*/

    // std::array<float, 3> center2({0,0,0});
    // std::pair<std::vector<float>, std::vector<int>> sphere =
    // generate_sphere_mesh(center2, 0.5); print_vertexes(&(sphere.first[0]),
    // sphere.first.size()/3, 3 );

    float fi = 0;
    Sphere<> sphere1;
    std::cout << "pi = " << sphere1.area()/4.0 << std::endl;

    Circle<> circle;
    //circle.generate_vertexes();
    glfwTerminate();
    return 0;
}
