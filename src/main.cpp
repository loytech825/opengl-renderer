#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "ShaderProgram.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "CameraController.hpp"
#include "Model.hpp"

#include "Framebuffer.hpp"
#include "Core.hpp"

#include "AppLayer.hpp"
#include "MainLayer.hpp"

void scene_resize(int width, int height, CameraController& cam, Framebuffer& fbuffer);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{

    {
    GLFWwindow* window = Core::init(SCR_WIDTH, SCR_HEIGHT, "LearnOpengl");

    if(!window) return -1;
    
    std::unique_ptr<MainLayer> main_layer = std::make_unique<MainLayer>(SCR_WIDTH, SCR_HEIGHT);

    double dt = 0;

    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    std::cout << "starting loop!\n";
    while (main_layer->is_running() && !glfwWindowShouldClose(window))
    {
        //while(auto err = glGetError());
        float now = glfwGetTime();

        main_layer->on_update(dt);

        Core::init_frame();

        main_layer->on_render();

        Core::end_frame(window);

        dt = glfwGetTime() - now;
        while(auto err = glGetError()) std::cout << err << "\n";
    }
    }
    Core::shutdown();
    return 0;
}