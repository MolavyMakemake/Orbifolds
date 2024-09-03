#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"

int main()
{
    window::run();

    glfwTerminate();
    return 0;
}