#include "opengl_test.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    print("Window created!");

    std::vector<Shape<float> *> shapes1;
    std::vector<Shape<double> *> shapes2;
    initialize_shapes(shapes1);
    //initialize_shapes(shapes2);
    print_vertexes(shapes1[0]->get_vertexes());
    //print_vertexes(shapes2[0]->get_vertexes());

    // glad takes care of opengl function pointers
    // you need this before you call any opengl functions
    // you have to do this after glfwMAkeContextCurrent(window)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);
        glfwPollEvents(); // check if any events happend (mouse press, key
                          // pres)
        //display(shapes2);
        display(shapes1);
        // draw_sphere(center, 0.5, fi);
        // Shape<float> test_sphere = static_cast<Shape<float>>(sphere1);
        // draw(sphere1, uniform_color_shader, {0.2, 0.2, 0.2}, {0.4, 0.5, 0},
        //     {0.5, 0.5, 0.5}, fi, {0.2, 0.4, 0.5, 0.7});

        // The glfwSwapBuffers will swap the color buffer (a large buffer
        // that contains color values for each pixel in GLFW’s window) that
        // has been used to draw in during this iteration and show it as
        // output to the screen.
        glfwSwapBuffers(window);
        glClearColor(0.2f, 0.3f, 0.3f,
                     1.0f); // set which color to clear the screen with
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
        // GL_STENCIL_BUFFER_BIT. set which buffer to use to clear the
        // screen
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // std::cout << "pi = " << sphere1.area() / 4.0 << std::endl;
    glfwTerminate();
    return 0;
}
