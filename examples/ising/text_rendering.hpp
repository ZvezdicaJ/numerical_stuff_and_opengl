#pragma once
#include <iostream>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <GLFW/glfw3.h>

class Text {

  private:
    FT_Library library;
    FT_Face face;
    unsigned horizontal_device_resolution =
        176; // 12.5 and 1920*1080 - autodetect in future
    unsigned vertical_device_resolution = 0;
    /*
      typedef struct {
      int Width, Height; // Video resolution
      int RedBits; // Number of red bits
      int GreenBits; // Number of green bits
      int BlueBits; // Number of blue bits
      } GLFWvidmode;*/

    GLFWvidmode mode;
    GLFWmonitor *primary;
    int monitor_widthMM, monitor_heightMM;

  public:
    Text(const std::string &font_file) {

        error = FT_Init_FreeType(&library);
        if (error) {
            std::cerr << "An error occurred during library initialization..."
                      << std::endl;
        }
        error = FT_New_Face(library, font_file.c_str(), 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            std::cerr << "the font file could be opened and read,
                but it appears... that its font format is
                    unsupported !"<<std::endl;
        } else if (error) {
            std::cerr << " Font file could not... be opened or read, or it is "
                         "broken... "
                      << std::endl;
        }
        // only works for single monitor situations
        primary = glfwGetPrimaryMonitor();
        glfwGetMonitorPhysicalSize(primary, &monitor_widthMM,
                                   &monitor_heightMM);

        glfwGetDesktopMode(&mode);
        horizontal_device_resolution = mode.width / (monitor_widthMM / 25.4);
        vertical_device_resolution = mode.height / (monitor_heightMM / 25.4);

        error = FT_Set_Char_Size(
            face,                         /* handle to face object           */
            0,                            /* char_width in 1/64th of points  */
            16 * 64,                      /* char_height in 1/64th of points */
            horizontal_device_resolution, /* horizontal device
                                             resolution    */
            vertical_device_resolution);  /* vertical device resolution
                                           */
    }

    /*
      The character widths and heights are specified in 1/64th of points. A
      point is a physical distance, equaling 1/72th of an inch. Normally, it is
      not equivalent to a pixel.
      Value of 0 for the character width means ‘same
      as character height’, value of 0 for the character height means ‘same as
      character width’. Otherwise, it is possible to specify different character
      widths and heights.
      The horizontal and vertical device resolutions are
      expressed in dots-per-inch, or dpi. Standard values are 72 or 96 dpi for
      display devices like the screen. The resolution is used to compute the
      character pixel size from the character point size.
      Value of 0 for the
      horizontal resolution means ‘same as vertical resolution’, value of 0 for
      the vertical resolution means ‘same as horizontal resolution’. If both
      values are zero, 72 dpi is used for both dimensions. The first argument is
      a handle to a face object, not a size object.
    */
    void set_char_size(int width, int height) {

        error = FT_Set_Char_Size(
            face,                         /* handle to face object           */
            width * 64,                   /* char_width in 1/64th of points  */
            height * 64,                  /* char_height in 1/64th of points */
            horizontal_device_resolution, /* horizontal device resolution    */
            vertical_device_resolution);  /* vertical device resolution      */
    }
};
