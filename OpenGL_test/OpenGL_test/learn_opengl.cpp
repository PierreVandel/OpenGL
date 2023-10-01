#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
//REMINDER
// Vertex data -> vertex shader > geometry shader > fragment shader
//A shader program object is the final linked version of multiple shaders combined.
//  To use the recently compiled shaders we have to link them to a shader program object
//  and then activate this shader program when rendering objects.

// VBO = Vertex Buffer Object : store vertices in the GPU's memory
// VAO = Vertex Array Object : ??? .Allow us to bind the vertex description of an object
// EBO = Element Buffer Object : 

//gl_Position is the output of the vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



int main()
{
    
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    glfwInit();

    //Since the focus of this book is on OpenGL version 3.3 we'd like to tell GLFW that 3.3 is the OpenGL 
    //version we want to use. This way GLFW can make the proper arrangements when creating the OpenGL context.
    //This ensures that when a user does not have the proper OpenGL version GLFW fails to run.
    //We set the major and minor version both to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //use the core-profile means we'll get access to a smaller subset of OpenGL features without
    //backwards-compatible features we no longer need.

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // WINDOW CREATION ------------------------------------
    //The function returns a GLFWwindow object (Reference ? Adress memory ?) that we'll later need for other GLFW operations.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // The adress of window is given and not directly the value

    // We can register a callback function on the window that gets called each time the window is resized.
    // To set a callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // LOAD ALL OPENGL FUNCTION POINTERS ------------------------------------
    //Initialize GLAD before calling GLWF functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // VERTEX SHADER
    // We create the type of shader we want
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach the shader source code to the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);// second arg : how many string we are passing as a source code, third arg : actual source code of the vertex shader
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // FRAGMENT SHADER
    // Compiling the fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // SHADER PROGRAM
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // We no longer need vertex and fragment shader anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //float vertices[] = {
    //-0.5f, -0.5f, 0.0f,
    // 0.5f, -0.5f, 0.0f,
    // 0.0f,  0.5f, 0.0f
    //};

    // When using an EBO
    float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f,   // top left
     1.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle
        0, 4, 1
    };

    int verticesLength = std::extent<decltype(indices)>::value;
    int numberTriangles = verticesLength * 3;

    // The Vertex Shader Object allow us to store a larg number of vertices in the GPU memory
    unsigned int VBO, VAO, EBO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the VBO Buffer to a group of buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy the data into the current bound buffer
    // GL_STATIC_DRAW means the data is set only once and used many times.check GL_STREAM_DRAW and GL_DYNAMIC_DRAW

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // RENDER LOOP ------------------------------------
    // The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed
    //  to close. If so, the function returns true and the render loop stops running, after which we can close 
    // the application.
    while (!glfwWindowShouldClose(window))
    {

        // input
        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3); // To draw the simple triangle using VBO
        glDrawElements(GL_TRIANGLES, verticesLength, GL_UNSIGNED_INT, 0);



        // This functions will swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) 
        // that is used to render to during this render iteration and show it as output to the screen.
        glfwSwapBuffers(window);

        // function checks if any events are triggered (like keyboard input or mouse movement events), updates the
        //  window state, and calls the corresponding functions (which we can register via callback methods).
        glfwPollEvents();
    }

    // Clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}


// Functions called each time the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // The first two parameters of glViewport set the location of the lower left corner of the window. 
    // The third and fourth parameter set the width and height of the rendering window in pixels, 
    // which we set equal to GLFW's window size.
    glViewport(0, 0, width, height);
}

// Function which query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int a = main();