#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
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
#include <stb/stb_image.h>
#include "Mesh.h"


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
        auto ctx = ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io1 = ImGui::GetIO(); (void)io1;

        ImGui_ImplGlfw_InitForOpenGL(glfwWindow1, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        Mesh mesh(MESH_PM, 41, 41);
        mesh.texture.id = textureFromFile("rendering/wallpaper.png", "../appdata/");
        Shader shader("shader.vert", "shader.frag");
        Camera camera;
        
        camera.Bake(window1_width, window1_height);
        shader.Activate();
        glUniform1i(shader.Loc("tex"), 0);
        glUniformMatrix4fv(shader.Loc("camera"), 1, GL_FALSE, glm::value_ptr(camera.mat));

        Diagnostics<1> diagnostics(500);
        glClearColor(0, 0, 0, 0);
        glEnable(GL_DEPTH_TEST);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glPolygonMode(GL_FRONT, GL_FILL);
        while (!glfwWindowShouldClose(glfwWindow1)) {
            diagnostics[0]++;
            diagnostics.tick();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (input.dx || input.dy || input.dz) {
                camera.rotate(0.04f * glm::vec3(input.dx, input.dy, 0));
                camera.radius += 0.04f * input.dz;
                camera.Bake(window1_width, window1_height);
                glUniformMatrix4fv(shader.Loc("camera"), 1, GL_FALSE, glm::value_ptr(camera.mat));
            }

            if (input.it || input.play) {
                for (int i = 0; i < std::max<int>(input.play, 1); i++)
                    mesh.iterate(.1f);
                mesh.Update();
            }

            mesh.Draw();

            // Draw GUI
            // --------

            ImGui::SetCurrentContext(ctx);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            gui_main(&input, &mesh);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // -

            glfwSwapBuffers(glfwWindow1);

            input_clear();
            glfwPollEvents();
        }

        glfwDestroyWindow(glfwWindow1);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
}