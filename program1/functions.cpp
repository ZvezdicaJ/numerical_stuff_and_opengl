#include "opengl_test.hpp"

bool check_vertex_shader(const unsigned shader_) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

bool check_fragment_shader(const unsigned shader_) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

bool check_shader_program(const unsigned shaderProgram) {
    // check if shader successfully compiled
    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

void pause() {
    bool run = true;
    while (run) {
        if (std::cin.get())
            run = false;
    }
}

void print(std::string str) { std::cout << str << std::endl; }

void OnPlusPressed(GLFWwindow *window) {
    // glfwGetKey function takes the window as input together with a
    // key. The function returns whether this key is currently being pressed.
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width * 1.1, height * 1.1);
    }
}

void OnMinusPressed(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSize(window, width / 1.1, height / 1.1);
    }
}

void OnEnterPressed(GLFWwindow *window, unsigned &enter_count) {
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (enter_count == 1) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetWindowPos(window, 100, 100);
        }
        if (enter_count == 2) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetWindowPos(window, 100, 100);
        }
        enter_count++;
    }
}

void OnClosePressed(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void draw_triangle() {

    float vertices[] = {-1.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                        0.0f,  0.0f,  0.5f, 0.0f};

    // create vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer object
    unsigned int VBO;
    // generate new buffer
    glGenBuffers(1, &VBO); // generate new buffer

    // bind the buffer -> opengl is a state machine
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // save array vertices into buffer
    // sizeof returns size in bytes
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // load  vertex  shader and compile it
    glShaderSource(vertexShader, 1, &(shaders::test_vertex_shader), NULL);
    glCompileShader(vertexShader);
    // check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::test_fragment_shader), NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    // create shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    check_shader_program(shaderProgram);
    // tell opengl to use this program
    // remember, opengl is just a state machine!
    glUseProgram(shaderProgram);

    // once shaders are linked, you don't need them any more
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // We stil have to teel how vertex shader should interpret the array of data
    // we will bind.
    // Arguments are as follows:
    // 1. First argument tells to which vertex shader attribute we
    // are passing out data - 0 in our case, check vertex shader attribute
    // number!

    // 2. Size of vertex attribute - here we have points in 3d space - so
    // it's 3.

    // 3. Type of data - GL_FLOAT in this case.

    // 4. True or false depending whether coordinates should be normalized
    // so that each coordinate is between 0 and 1 (or -1 and 1 for signed data).

    // 5. Stride tells us the space between consecutive vertex attribute sets
    // (space between points) In out case it is 3*sizeof(float) - it is given in
    // bytes if you specify 0, opengl will automatically figure it out - this is
    // not recommended.

    // 6. offset to where the position data begins. If coordinates start at the
    // start of array it is 0, otherwise specify where the data start!

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // 3. now draw the object
    // first argument tells what to draw
    // second argument is starting index
    // third is how many indices to render - in this case 3 indices (3 points,
    // which form a single triangle)
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void draw_sphere() {

    float vertices[] = {-0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f,
                        0.0f,  0.5f, 0.0f,
                        0.0f,  0.5f, 0.0f,
                        -0.5f, -0.5f, 0.0f};
    // create vao
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // create and bind vbo
    unsigned int VBO;
    glGenBuffers(1, &VBO); // generate new buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create vertex shader
    unsigned int vertexShader;
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &(shaders::test_vertex_shader), NULL);
    glCompileShader(vertexShader);
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load fragment shader and compile it
    glShaderSource(fragmentShader, 1, &(shaders::test_fragment_shader), NULL);
    glCompileShader(fragmentShader);
    check_fragment_shader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    check_shader_program(shaderProgram);
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // 3. now draw the object
    // first argument tells what to draw
    // second argument is starting index
    // third is how many indices to render - in this case 3 indices (3 points,
    // which form a single triangle)
    glDrawArrays(GL_LINES, 0, 6);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
   
    glDrawArrays(GL_POINTS, 0, 6);
}
