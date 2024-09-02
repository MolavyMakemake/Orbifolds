#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "application.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "implot/implot.h"

#include <iostream>

#include "window.h"
#include "input.h"
#include <ini/ini.h>
#include "initools.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


namespace window {
    int window1_width, window1_height;
    GLFWwindow* glfwWindow1;

    void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (window == glfwWindow1) {
            window1_width = width;
            window1_height = height;
        }
    }

    void run() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindow1 = glfwCreateWindow(1080, 720, "Orbifolds", NULL, NULL);

        glfwGetWindowSize(glfwWindow1, &window1_width, &window1_height);
        glfwSetWindowSizeCallback(glfwWindow1, windowSizeCallback);

        if (glfwWindow1 == NULL) {
            std::cout << "Failed to create a window" << std::endl;
            glfwTerminate();
            return;
        }

        GLFWimage icon;
        icon.pixels = stbi_load(ini_getFilename("rendering/klein.png").c_str(), 
                                    &icon.width, &icon.height, 0, 4);
        glfwSetWindowIcon(glfwWindow1, 1, &icon);
        stbi_image_free(icon.pixels);

        glfwMakeContextCurrent(glfwWindow1);

        glfwSwapInterval(1);
        glfwSetKeyCallback(glfwWindow1, onKeyAction);
        glfwSetCharCallback(glfwWindow1, charCallback);


        // Init glad
        gladLoadGL();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io1 = ImGui::GetIO(); (void)io1;

        ImGui_ImplGlfw_InitForOpenGL(glfwWindow1, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        Diagnostics<1> diagnostics(500);
        glClearColor(0, 0, 0, 0);
        while (!glfwWindowShouldClose(glfwWindow1)) {
            diagnostics[0]++;
            diagnostics.tick();

            // Draw GUI
            // --------

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // -

            glfwSwapBuffers(glfwWindow1);
            glfwPollEvents();
        }

        glfwDestroyWindow(glfwWindow1);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
}