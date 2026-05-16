#include <glad/glad.h>
#include  <GLFW/glfw3.h>
#include "include/GLFWCallbacks.h"
#include "include/Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <string>

//
GLFWimage load_icon(const std::string& filepath);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// Fixes scaling problems on wayland linux
#ifdef __linux__
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif

    int width = 1920;
    int height = 1200;
    GLFWwindow* window = glfwCreateWindow(width, height, "Learning OpenGL", NULL, NULL);
    GLFWimage icons[1];
    icons[0] = load_icon("assets/icons/opengl.png");

    if (icons[0].pixels) { 
        glfwSetWindowIcon(window, 1, icons);
        stbi_image_free(icons[0].pixels);
    }

    if (window == NULL)
    {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Get the actual framebuffer size (accounts for DPI scaling)
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    WindowState* state{};
    glfwSetWindowUserPointer(window, &state);

    glfwSetKeyCallback(window, key_callback);

    // Uses our new shader object cutting down on boilerplate code
    Shader shader{ "assets/shaders/shader.vert","assets/shaders/shader.frag" };

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right (0)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left  (1)
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // top right (2)
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f    // top left    (3)
    };

    unsigned int indices[]{
        0,1,2,
        0,2,3
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

// A simple function to load an image file into a GLFWimage struct
GLFWimage load_icon(const std::string& filepath) {
    GLFWimage image;
    int channels;

    const char* filepath_c_str = filepath.c_str();
    image.pixels = stbi_load(filepath_c_str, &image.width, &image.height, &channels, 4);

    if (image.pixels == nullptr) {
        std::cerr << "Error: Failed to load icon file: " << filepath << std::endl;
        image.width = 0;
        image.height = 0;
    }
    return image;
}