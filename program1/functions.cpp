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


void OnPlusPressed(GLFWwindow *window) {
    // glfwGetKey function takes the window as input together with a
    // key. The function returns whether this key is currently being pressed.
  if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowSize(window, width*1.1, height*1.1);
  }
}

void OnMinusPressed(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowSize(window, width/1.1, height/1.1);
  }
}

void OnEnterPressed(GLFWwindow *window, unsigned& enter_count) {
  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    if(enter_count==1) {
      int width, height;
      glfwGetWindowSize(window, &width, &height);
      glfwSetWindowPos(window, 100, 100);
    }
    if(enter_count==2) {
      int width, height;
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


