enum class RENDER_TYPE { UNIFORM_COLOR = 0, CUSTOM_COLOR = 1 };

template <RENDER_TYPE T>
class Shader {
  protected:
    unsigned shader_program[3]; /**< shader program array depending on which
                                  vertex size is chosen*/
    bool shaders_compiled[3] = {
        false, false,
        false}; /**< true/false depending on whether the shaders were compiled*/

    void compile_shaders();

  public:
    Shader() { compile_shaders(); };
    Shader(Shader &&) = delete;
    Shader &operator=(Shader &&) = delete;
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    unsigned get_shader_program(int i) { return shader_program[i]; }
};

template <>
inline void Shader<RENDER_TYPE::UNIFORM_COLOR>::compile_shaders() {
    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::uniform_fragment_shader),
                   NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    for (int i = 0; i < 3; i++) {
        // create empty shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        const char *source = shaders::uniform_vertex_shaders[i].c_str();
        // std::cout << source << std::endl;
        glShaderSource(vertexShader, 1, &source, NULL);
        glCompileShader(vertexShader);

        // check if successfully compiled
        check_vertex_shader(vertexShader);

        shader_program[i] = glCreateProgram();
        glAttachShader(shader_program[i], vertexShader);
        glAttachShader(shader_program[i], fragmentShader);
        glLinkProgram(shader_program[i]);
        check_shader_program(shader_program[i]);
        shaders_compiled[i] = true;
        // glUseProgram(shaderProgram);
        glDeleteShader(vertexShader);
    }
    glDeleteShader(fragmentShader);
}

template <>
inline void Shader<RENDER_TYPE::CUSTOM_COLOR>::compile_shaders() {
    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::custom_fragment_shader), NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    for (int i = 0; i < 3; i++) {
        // create empty shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // load  vertex  shader and compile it
        const char *source = shaders::custom_vertex_shaders[i].c_str();
        glShaderSource(vertexShader, 1, &source, NULL);
        glCompileShader(vertexShader);
        // check if successfully compiled
        check_vertex_shader(vertexShader);

        shader_program[i] = glCreateProgram();
        glAttachShader(shader_program[i], vertexShader);
        glAttachShader(shader_program[i], fragmentShader);
        glLinkProgram(shader_program[i]);
        check_shader_program(shader_program[i]);
        shaders_compiled[i] = true;
        glUseProgram(shader_program[i]);
        glDeleteShader(vertexShader);
    }
    glDeleteShader(fragmentShader);
}
