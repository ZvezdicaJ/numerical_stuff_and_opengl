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
    OnMinusPressed(window);
    OnPlusPressed(window);
    OnClosePressed(window);
    glfwPollEvents(); // check if any events happend (mouse press, key press)
    glfwSwapBuffers(window);
  }

  //initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl; return -1;
  }

  return 0;
  }
