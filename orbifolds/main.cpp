#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "application.h"

int main()
{
    std::thread application_thread(&application::run);

    window::run();

    application::terminate();
    application_thread.join();

    glfwTerminate();
    return 0;
}