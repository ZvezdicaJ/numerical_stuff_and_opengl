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
    // print("Window created!");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //    std::vector<Shape<float> *> shapes1;
    // Sphere<float> sphere1;
    // std::vector<Shape<double> *> shapes2;
    // initialize_shapes(shapes1);
    // initialize_shapes(shapes2);
    // std::vector<float> vert = shapes1[0]->get_vertexes();
    // print_vertexes(vert, vert.size()/2, 2);
    // print_vertexes(shapes2[0]->get_vertexes());

    // glad takes care of opengl function pointers
    // you need this before you call any opengl functions
    // you have to do this after glfwMAkeContextCurrent(window)

    Shader<RENDER_TYPE::UNIFORM_COLOR> uniform_shader;
    Shader<RENDER_TYPE::CUSTOM_COLOR> color_shader;
    // Shader<RENDER_TYPE::CUSTOM_COLOR> shader2;

    // Circle<float> circle2;
    // circle2.generate_random_colors();
    // std::vector<float> vert = circle2.get_vertexes();
    // print_vertexes(vert, vert.size() / 2, 2);
    // std::cout << "\n\n" << std::endl;
    // Circle<float> circle2;
    // std::vector<float> vert2 = circle2.get_vertexes();
    // print_vertexes(vert2, vert2.size() / 2, 2);
    
    Disk<float> disk1;
    disk1.generate_random_colors();
    // aligned_vector<float> vert = rect1.get_filling_vertexes();
    // std::cout << "rect filing vertexes: " << std::endl;
    // print_vertexes(vert, vert.size() / 2, 2);
    Sphere<double> sphere1(1000);
    Box<float> box1;
    SquareBoard<float> board1(50);
    board1.generate_random_colors();
    Star<float> star1(10, 0.5);
    float fi = 0.0;
    while (!glfwWindowShouldClose(window)) {
        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);
        glfwPollEvents(); // check if any events happend (mouse press, key
                          // press)
                          // display(shapes2);

        draw(disk1, uniform_shader, {0.5, 0.5, 0.5}, {0.6, 0.6, 0.6}, {0, 0, 1},
             fi, {0.5, 0.5, 0.5, 0.8});
        draw_wireframe(disk1, uniform_shader, {0.5, 0.5, 0.5}, {0.6, 0.6, 0.6},
                       {0, 0, 1}, fi, {0.5, 0, 0, 0.8});
        //        draw_2d_object(board1, uniform_shader, {0.5, 0.5, 0.5}, {0, 0,
        //        0},
        //               {1, 1, 1}, 0, {0.5, 0.5, 0.5, 0.5});
        draw(board1, uniform_shader, {0.5, 0.5, 0.5}, {0, -0.6, 0.6}, {0, 0, 1},
             fi, {0.5, 0.5, 0.5, 0.5});
        draw_2d_object(board1, color_shader, {0.5, 0.5, 0.5}, {0.6, 0.6, 0},
                       {0, 0, 1}, fi);

        draw_2d_object(star1, uniform_shader, {0.5, 0.5, 0.5}, {0, 0, 0},
                       {0, 0, 1}, fi, {0.2, 0.5, 0.2, 1.0});

        draw(star1, uniform_shader, {0.5, 0.5, 0.5}, {0, 0, 0}, {0, 0, 1}, fi,
             {0.5, 0.0, 0.0, 1.0});

        draw(sphere1, uniform_shader, {0.2, 0.2, 0.2},
             {0.7f * std::cos(fi), 0, 0.7f * std::sin(fi)}, {1, 1, 1}, fi,
             {0, 0, 0.5, 0.2});

        draw_wireframe(sphere1, uniform_shader, {0.2, 0.2, 0.2},
                       {0.7f * std::cos(fi), 0, 0.7f * std::sin(fi)}, {1, 1, 1},
                       fi, {0, 0, 0.5, 0.9});
        /*
draw(star1, shader1, {0.5, 0.5, 0.5}, {0, 0, 0}, {1, 1, 1}, fi,
{0.5, 0, 0.5, 0.5});

draw_wireframe(star1, shader1, {0.5, 0.5, 0.5}, {0, 0, 0}, {1, 1, 1},
fi, {0, 0, 0, 1.0});


draw_wireframe(sphere1, shader1, {0.5, 0.5, 0.5}, {0, 0, 0}, {1, 1, 1},
fi, {0.0, 0, 0, 0.2});
*/
        // draw_wireframe(disk1, shader1, {0.5, 0.5, 0.5}, {0, 0, 0}, {1,
        // 1, 1},
        //              fi);
        fi += 0.02;
        // The glfwSwapBuffers will swap the color buffer (a large buffer
        // that contains color values for each pixel in GLFW’s window) that
        // has been used to draw in during this iteration and show it as
        // output to the screen.
        glfwSwapBuffers(window);
        glClearColor(1.0f, 1.0f, 1.0f,
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
