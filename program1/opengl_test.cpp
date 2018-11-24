#include "opengl_test.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    print("Window created!");

    // glad takes care of opengl function pointers
    // you need this before you call any opengl functions
    // you have to do this after glfwMAkeContextCurrent(window)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

    while (!glfwWindowShouldClose(window)) {
        OnMinusPressed(window);
        OnPlusPressed(window);
        OnClosePressed(window);
        glfwPollEvents(); // check if any events happend (mouse press, key
                          // press)

        // The glfwSwapBuffers will swap the color buffer (a large buffer that
        // contains color values for each pixel in GLFW’s window) that has been
        // used to draw in during this iteration and show it as output to the
        // screen.
        glfwSwapBuffers(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set which color to clear the screen with
        //GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT.
        // set which buffer to use to clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
                        -0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f,
                        0.0f, 0.5f, 0.0f
    };

    unsigned int VBO;
    // generate new buffer
    glGenBuffers(1, &VBO);// generate new buffer

    // bind the buffer -> opengl is a state machine
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // save array vertices into buffer
    //sizeof returns size in bytes
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

     // create empty shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // load  vertex  shader and compile it
   glShaderSource(vertexShader, 1, &(shaders::test_vertex_shader), NULL);
    glCompileShader(vertexShader);
    //check if successfully compiled
    check_vertex_shader(vertexShader);

    // create empty fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //load fragment shader and compile it
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

    // We stil have to teel how vertex shader should interpret the array of data we will bind.
    //Arguments are as follows:
    // 1. First argument tells to which vertex shader attribute we
    // are passing out data - 0 in out case, check vertex shader attribute number!

    // 2. Size of vertex attribute - here we have points in 3d space - so it's 3.

    // 3. Type of data - GL_FLOAT in this case.

    // 4. True or false depending whether coordinates should be normalized
    // so that each coordinate is between 0 and 1 (or -1 and 1 for signed data).

    // 5. Stride tells us the space between consecutive vertex attribute sets (space between points)
    // In out case it is 3*sizeof(float) - it is given in bytes
    // if you specify 0, opengl will automatically figure it out - this is not recommended.

    // 6. offset to where the position data begins. If coordinates start at the start
    // of array it is 0, otherwise specify where the data start!

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glfwTerminate();
    return 0;
}
