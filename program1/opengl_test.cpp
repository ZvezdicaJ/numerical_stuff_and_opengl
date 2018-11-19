#include "opengl_test.hpp"

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1;
  }

  glfwMakeContextCurrent(window);
  print("Window created!");

  while (!glfwWindowShouldClose(window)) {
    int enter_pressed=0;
    OnEnterPressed(window, enter_pressed);
    OnClosePressed(window);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  pause();
  glfwSetWindowSize(window, 640, 480);
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  glfwSetWindowSize(window, width*2, height*2);
  print("window resized!");

  pause();
  glfwHideWindow(window);
  print("window hidden");

  pause();
  glfwShowWindow(window);
  print("Window showed again");

  pause();
  glfwGetWindowSize(window, &width, &height);
  glfwSetWindowPos(window, 100, 100);
  print("window moved and resized");

  pause();

  //initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl; return -1;
  }

  return 0;
  }
