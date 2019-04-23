#pragma once

#include "nanogui/nanogui.h"
#include "callback_functions.hpp"

class callback_function_class {

  private:
    nanogui::Screen *screen;

  public:
    callback_function_class(nanogui::Screen *screen_) : screen(screen_){};
    callback_function_class(callback_function_class &&) = delete;
    callback_function_class &operator=(callback_function_class &&) = delete;
    callback_function_class(const callback_function_class &) = delete;
    callback_function_class &
    operator=(const callback_function_class &) = delete;

    void cursorPosCallbackEvent(GLFWwindow *, double x, double y) {
        screen->cursorPosCallbackEvent(x, y);
    };

    void mouseButtonCallbackEvent(GLFWwindow *, int button, int action,
                                  int modifiers) {
        screen->mouseButtonCallbackEvent(button, action, modifiers);
    };

    void keyCallbackEvent(GLFWwindow *, int key, int scancode, int action,
                          int mods) {
        screen->keyCallbackEvent(key, scancode, action, mods);
    }

    void charCallbackEvent(GLFWwindow *, unsigned int codepoint) {
        screen->charCallbackEvent(codepoint);
    };

    void dropCallbackEvent(GLFWwindow *, int count, const char **filenames) {
        screen->dropCallbackEvent(count, filenames);
    };

    void scrollCallbackEvent(GLFWwindow *, double x, double y) {
        screen->scrollCallbackEvent(x, y);
    };

    void resizeCallbackEvent(GLFWwindow *, int width, int height) {
        screen->resizeCallbackEvent(width, height);
    };
};
