#pragma once
#include <memory>
#include "nanogui/nanogui.h"
#include "apex_memmove.h"
#include "type_definitions.hpp"
#include "convex_hull.hpp"

#include "shaders.hpp"
#include "shader_class.hpp"
#include "auxiliary_functions.hpp"
#include "print_functions.hpp"
#include "drawing_functions.hpp"
#include "display_functions.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <mgl2/mgl.h>

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

#include "display_functions.hpp"
#include "ising_shader.hpp"
#include "spin_array.hpp"
#include "text_rendering.hpp"

#include "ising_windows.hpp"

/*void settings_window(nanogui::FormHelper *, GLFWwindow *, IsingModel<float> &,
                     char &, std::vector<float> &, std::vector<float> &);
*/

class ExampleApplication : public nanogui::Screen {
  public:
    ExampleApplication()
        : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test") {
        using namespace nanogui;

        /////////////////////////////////////////////////////////

        Window *window = new Window(this, "Button demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        /* No need to store a pointer, the data structure will be automatically
           freed when the parent window is deleted */
        new Label(window, "Push buttons", "sans-bold");

        Button *b = new Button(window, "Plain button");
        b->setCallback([] { std::cout << "pushed!" << std::endl; });
        b->setTooltip("short tooltip");

        /* Alternative construction notation using variadic template */
        b = window->add<Button>("Styled", ENTYPO_ICON_ROCKET);
        b->setBackgroundColor(Color(0, 0, 255, 25));
        b->setCallback([] { std::cout << "pushed!" << std::endl; });
        b->setTooltip(
            "This button has a fairly long tooltip. It is so long, in "
            "fact, that the shown text will span several lines.");

        new Label(window, "Toggle buttons", "sans-bold");
        b = new Button(window, "Toggle me");
        b->setFlags(Button::ToggleButton);
        b->setChangeCallback([](bool state) {
            std::cout << "Toggle button state: " << state << std::endl;
        });

        new Label(window, "Radio buttons", "sans-bold");
        b = new Button(window, "Radio button 1");
        b->setFlags(Button::RadioButton);
        b = new Button(window, "Radio button 2");
        b->setFlags(Button::RadioButton);

        new Label(window, "A tool palette", "sans-bold");
        Widget *tools = new Widget(window);
        tools->setLayout(
            new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 6));

        b = new ToolButton(tools, ENTYPO_ICON_CLOUD);
        b = new ToolButton(tools, ENTYPO_ICON_CONTROLLER_FAST_FORWARD);
        b = new ToolButton(tools, ENTYPO_ICON_COMPASS);
        b = new ToolButton(tools, ENTYPO_ICON_INSTALL);

        new Label(window, "Popup buttons", "sans-bold");
        PopupButton *popupBtn =
            new PopupButton(window, "Popup", ENTYPO_ICON_EXPORT);
        Popup *popup = popupBtn->popup();
        popup->setLayout(new GroupLayout());
        new Label(popup, "Arbitrary widgets can be placed here");
        new CheckBox(popup, "A check box");
        // popup right
        popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
        Popup *popupRight = popupBtn->popup();
        popupRight->setLayout(new GroupLayout());
        new CheckBox(popupRight, "Another check box");
        // popup left
        popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
        popupBtn->setSide(Popup::Side::Left);
        Popup *popupLeft = popupBtn->popup();
        popupLeft->setLayout(new GroupLayout());
        new CheckBox(popupLeft, "Another check box");

        /////////////////////////////////////////////////////////

        window = new Window(this, "Basic widgets");
        window->setPosition(Vector2i(200, 15));
        window->setLayout(new GroupLayout());

        new Label(window, "Message dialog", "sans-bold");
        tools = new Widget(window);
        tools->setLayout(
            new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 6));
        b = new Button(tools, "Info");
        b->setCallback([&] {
            auto dlg =
                new MessageDialog(this, MessageDialog::Type::Information,
                                  "Title", "This is an information message");
            dlg->setCallback([](int result) {
                std::cout << "Dialog result: " << result << std::endl;
            });
        });
        b = new Button(tools, "Warn");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Warning,
                                         "Title", "This is a warning message");
            dlg->setCallback([](int result) {
                std::cout << "Dialog result: " << result << std::endl;
            });
        });
        b = new Button(tools, "Ask");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Warning,
                                         "Title", "This is a question message",
                                         "Yes", "No", true);
            dlg->setCallback([](int result) {
                std::cout << "Dialog result: " << result << std::endl;
            });
        });

        new Label(window, "File dialog", "sans-bold");
        tools = new Widget(window);
        tools->setLayout(
            new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 6));
        b = new Button(tools, "Open");
        b->setCallback([&] {
            std::cout << "File dialog result: "
                      << file_dialog({{"png", "Portable Network Graphics"},
                                      {"txt", "Text file"}},
                                     false)
                      << std::endl;
        });
        b = new Button(tools, "Save");
        b->setCallback([&] {
            std::cout << "File dialog result: "
                      << file_dialog({{"png", "Portable Network Graphics"},
                                      {"txt", "Text file"}},
                                     true)
                      << std::endl;
        });

        new Label(window, "Combo box", "sans-bold");
        new ComboBox(window, {"Combo box item 1", "Combo box item 2",
                              "Combo box item 3"});
        new Label(window, "Check box", "sans-bold");
        CheckBox *cb = new CheckBox(window, "Flag 1", [](bool state) {
            std::cout << "Check box 1 state: " << state << std::endl;
        });
        cb->setChecked(true);
        cb = new CheckBox(window, "Flag 2", [](bool state) {
            std::cout << "Check box 2 state: " << state << std::endl;
        });
        new Label(window, "Progress bar", "sans-bold");
        mProgress = new ProgressBar(window);

        new Label(window, "Slider and text box", "sans-bold");

        Widget *panel = new Widget(window);
        panel->setLayout(
            new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

        Slider *slider = new Slider(panel);
        slider->setValue(0.5f);
        slider->setFixedWidth(80);

        TextBox *textBox = new TextBox(panel);
        textBox->setFixedSize(Vector2i(60, 25));
        textBox->setValue("50");
        textBox->setUnits("%");
        slider->setCallback([textBox](float value) {
            textBox->setValue(std::to_string((int)(value * 100)));
        });
        slider->setFinalCallback([&](float value) {
            std::cout << "Final slider value: " << (int)(value * 100)
                      << std::endl;
        });
        textBox->setFixedSize(Vector2i(60, 25));
        textBox->setFontSize(20);
        textBox->setAlignment(TextBox::Alignment::Right);

        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////

        performLayout();
    }

    ~ExampleApplication() {}

    virtual bool keyboardEvent(int key, int scancode, int action,
                               int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Animate the scrollbar */
        mProgress->setValue(std::fmod((float)glfwGetTime() / 10, 1.0f));

        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() { using namespace nanogui; }

    ExampleApplication(IsingModel<float> *model,
                       SpinArray<float> *spin_array = NULL)
        : ExampleApplication() {
        // spin_array->set_clickable_square(this->glfwWindow());
    }

  private:
    nanogui::ProgressBar *mProgress;
};
