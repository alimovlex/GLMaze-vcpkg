#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct WindowState
{
    bool wireframe = false;
};

// automatically resizes the viewport when the window size is changed
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void key_callback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        auto* s = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
        if (!s) return; // or assert/log
    	s->wireframe = !s->wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, s->wireframe ? GL_LINE : GL_FILL);
        
    }
}

// Checks for input every frame in render loop
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
