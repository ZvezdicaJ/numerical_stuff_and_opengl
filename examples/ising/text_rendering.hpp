#pragma once
#include <iostream>
#include <map>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <GLFW/glfw3.h>
#include "text_shaders.hpp"

class Text {

  private:
    FT_Library library;
    FT_Face face;
    unsigned horizontal_device_resolution =
        176; // 12.5 and 1920*1080 - autodetect in future
    unsigned vertical_device_resolution = 176;
    /*
      typedef struct {
      int Width, Height; // Video resolution
      int RedBits; // Number of red bits
      int GreenBits; // Number of green bits
      int BlueBits; // Number of blue bits
      } GLFWvidmode;*/

    GLFWmonitor *primary;
    int monitor_widthMM, monitor_heightMM;

    GLuint VAO, VBO;
    struct Character {
        GLuint TextureID;   // ID handle of the glyph texture
        glm::ivec2 Size;    // Size of glyph
        glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
        GLuint Advance;     // Offset to advance to next glyph
    };
    std::map<GLchar, Character> characters;
    Shader<RENDER_TYPE::CUSTOM> text_shader;

  public:
    Text(const std::string &font_file)
        : text_shader(basic_text_shaders::vertex_shader, "",
                      basic_text_shaders::fragment_shader) {

        int error = FT_Init_FreeType(&library);
        if (error) {
            std::cerr << "An error occurred during library initialization..."
                      << std::endl;
        }
        error = FT_New_Face(library, font_file.c_str(), 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            std::cerr << "the font file could be opened and read, but it "
                         "appears... that its font format is unsupported !"
                      << std::endl;
        } else if (error) {
            std::cerr << " Font file could not... be opened or read, or it is "
                         "broken... "
                      << std::endl;
        }
        // only works for single monitor situations
        primary = glfwGetPrimaryMonitor();
        glfwGetMonitorPhysicalSize(primary, &monitor_widthMM,
                                   &monitor_heightMM);

        const GLFWvidmode *mode = glfwGetVideoMode(primary);

        horizontal_device_resolution = mode->width / (monitor_widthMM / 25.4);
        vertical_device_resolution = mode->height / (monitor_heightMM / 25.4);
        std::cout << "horizontal_device_resolution: "
                  << horizontal_device_resolution << std::endl;
        std::cout << "vertical_device_resolution: "
                  << vertical_device_resolution << std::endl;

        error = FT_Set_Char_Size(
            face,                         /* handle to face object           */
            0,                            /* char_width in 1/64th of points  */
            12*64,                      /* char_height in 1/64th of points */
            horizontal_device_resolution, /* horizontal device
                                             resolution    */
            vertical_device_resolution);  /* vertical device resolution
                                           */

        // Disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        /*Within the for loop we list over all the 128 characters of the ASCII
          set and retrieve their corresponding character glyphs. For each
          character, we generate a texture, set its options and store its
          metrics. What is interesting to note here is that we use GL_RED as the
          texture’s internalFormat and format arguments. The bitmap generated
          from the glyph is a grayscale 8-bit image where each color is
          represented by a single byte. For this reason we’d like to store each
          byte of the bitmap buffer as a texture’s color value. We accomplish
          this by creating a texture where each byte corresponds to the texture
          color’s red component (first byte of its color vector).*/

        for (GLubyte c = 0; c < 128; c++) {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph"
                          << std::endl;
                continue;
            } // Generate texture

            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            /*
              GL_TEXTURE_2D  - 2d image

             level: 0  - Specifies the level-of-detail number. Level 0 is the
             base image level.

             GL_RED - Specifies the number of color components in the
             texture.Each element is a single red component. The GL converts it
             to floating point and assembles it into an RGBA element by
             attaching 0 for green and blue, and 1 for alpha.

             face->glyph->bitmap.width - width
             face->glyph->bitmap.rows - height

             border: 0  This value must be 0.
             format: GL_RED -  Specifies the format of the pixel data.
             type:  GL_UNSIGNED_BYTE -  Specifies the data type of the pixel
             data: face->glyph->bitmap.buffer
            */
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                         face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                         face->glyph->bitmap.buffer);
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x};
            characters.insert(std::pair<GLchar, Character>(c, character));
        }
        initialize_gl_buffers();
    }

    void initialize_gl_buffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL,
                     GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // UNBIND
        glBindVertexArray(0);             // UNBIND
    }

    /*
      The character widths and heights are specified in 1/64th of points. A
      point is a physical distance, equaling 1/72th of an inch. Normally, it
      is not equivalent to a pixel. Value of 0 for the character width means
      ‘same as character height’, value of 0 for the character height means
      ‘same as character width’. Otherwise, it is possible to specify
      different character widths and heights. The horizontal and vertical
      device resolutions are expressed in dots-per-inch, or dpi. Standard
      values are 72 or 96 dpi for display devices like the screen. The
      resolution is used to compute the character pixel size from the
      character point size. Value of 0 for the horizontal resolution means
      ‘same as vertical resolution’, value of 0 for the vertical resolution
      means ‘same as horizontal resolution’. If both values are zero, 72 dpi
      is used for both dimensions. The first argument is a handle to a face
      object, not a size object.
    */
    void set_char_size(int width, int height) {

        int error = FT_Set_Char_Size(
            face,                         /* handle to face object           */
            width * 64,                   /* char_width in 1/64th of points  */
            height * 64,                  /* char_height in 1/64th of points */
            horizontal_device_resolution, /* horizontal device
                                             resolution    */
            vertical_device_resolution);  /* vertical device resolution
                                           */
        if (error) {
            std::cerr << "failed to change the font size!" << std::endl;
        }
    }

    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
                    glm ::vec3 color, GLFWwindow *window) {
        // Activate corresponding render state
        unsigned shaderProgram = text_shader.get_shader_program();
        glUseProgram(shaderProgram);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width),
                                          0.0f, static_cast<GLfloat>(height));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.x,
                    color.y, color.z);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = characters[*c];
            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                {xpos, ypos + h, 0.0, 0.0}, {xpos, ypos, 0.0, 1.0},
                {xpos + w, ypos, 1.0, 1.0}, {xpos, ypos + h, 0.0, 0.0},
                {xpos + w, ypos, 1.0, 1.0}, {xpos + w, ypos + h, 1.0, 0.0}};
            /*           for (int i = 0; i < 6; i++) {
                for (int k = 0; k < 4; k++)
                    std::cout << vertices[i][k] << " ";
                std::cout << "\n" << std::endl;
                }*/
            // std::cout << "\n" << std::endl;
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number
            // of 1/64 pixels)
            x += (ch.Advance >> 6) *
                 scale; // Bitshift by 6 to get value in pixels (2^6= 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
