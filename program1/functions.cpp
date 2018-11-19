#include "opengl_test.hpp"

void pause() {
  bool run = true;
  while(run) {
    if(std::cin.get()) run=false;
  }
}

void print(std::string str) {
  std::cout<<str<<std::endl;
}

void OnEnterPressed(GLFWwindow *window, unsigned& enter_count) {
  if(glfwGetKey(window, GLFW_KEY_ENTETR) == GLFW_PRESS) {
    if(enter_count==0) {
      glfwSetWindowSize(window, 640, 480);
      int width, height;
      glfwGetWindowSize(window, &width, &height);
      glfwSetWindowSize(window, width*2, height*2);
    }
    if(enter_count==1) {
      unsigned width, height;
      glfwGetWindowSize(window, &width, &height);
      glfwSetWindowPos(window, 100, 100);
    }
    if(enter_count==2) {
      unsigned width, height;
      glfwGetWindowSize(window, &width, &height);
      glfwSetWindowPos(window, 100, 100);
    }
    enter_count++;
  }
}

void OnClosePressed(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}


pause();
glfwGetWindowSize(window, &width, &height);
glfwSetWindowPos(window, 100, 100);
print("window moved and resized");

pause();



