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

void scene_resize(int width, int height, CameraController& cam, Framebuffer& fbuffer);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int init_glwf_imgui(GLFWwindow*& window)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    #pragma region GLFW_SETUP    

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    #pragma endregion

    #pragma region IMGUI_SETUP

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    #pragma endregion

    return 0;
}

void cleanup()
{
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

int main()
{
    GLFWwindow* window;
    if(init_glwf_imgui(window) == -1) return -1;

    {
    ShaderProgram shader("shaders/vertex.glsl", "shaders/fragment.glsl");
 
    TextureManager tm;
    Model backpack("res/models/backpack/backpack.obj", tm);

    glm::vec3 light_dir(1, 1, 1);
    float fov = 45;

    double dt = 0;

    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    float render_choice;
    std::cout << sizeof(std::vector<Vertex>::iterator) << "\t" << sizeof(Vertex*) << "\t" << sizeof(unsigned int) << "\n";
    //FRAMEBUFFER for rendering to imguiwindow
    Framebuffer fbuffer(SCR_WIDTH, SCR_HEIGHT);
    CameraController cam(SCR_WIDTH, SCR_HEIGHT, 45);

    ImVec2 scene_size{SCR_WIDTH, SCR_HEIGHT};

    while (!glfwWindowShouldClose(window))
    {
        while(auto err = glGetError());
        float now = glfwGetTime();
        // input
        // -----
        processInput(window);

        //cam.update_view();
        //cam.update_proj(SCR_WIDTH, SCR_HEIGHT, fov);

        // render
        // ------
        fbuffer.bind();
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.bind();
        shader.set_uniform("u_model", model);

        glm::mat3 normal_matrix = glm::transpose(glm::inverse(model));
        shader.set_uniform("u_normal_matrix", normal_matrix);   

        shader.set_uniform("u_proj_view", cam.get_proj_x_view());
        shader.set_uniform("u_camera_pos", cam.get_pos());
        shader.set_uniform("u_choose_render", render_choice);
        glm::vec3 light_dir_n = glm::normalize(light_dir);
        shader.set_uniform("u_light_dir", light_dir_n);
        backpack.Draw(shader);

        glBindVertexArray(0);
        glUseProgram(0);


        //reset framebuffer for imgui rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();       
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode);


        ImGui::SetNextWindowSize({SCR_WIDTH, SCR_HEIGHT});
        ImGui::Begin("Scene");

        ImVec2 new_scene_size = ImGui::GetContentRegionAvail();

        if(!(new_scene_size.x == scene_size.x && new_scene_size.y == scene_size.y))
        {
            scene_resize(new_scene_size.x, new_scene_size.y, cam, fbuffer);
            scene_size = new_scene_size;
        }
        ImGui::Image((ImTextureRef)fbuffer.get_texture_handle(), scene_size, {0, 1}, {1, 0});

        if(ImGui::IsWindowFocused())
            //maybe set flag and update before all rendering code
            cam.update(dt);



        ImGui::End();

        ImGui::Begin("Light Dir");
        ImGui::DragFloat3("Dir", glm::value_ptr(light_dir), 1, -1, 1);

        const char* choices[] = {"Normal render", "Normals", "Frag to cam", "Frag to light", "Reflection", "Specular"};
        if(ImGui::BeginCombo("##combo", choices[(int)render_choice])){

            for(int i = 0; i < 6; i++)
            {
                bool is_selected = render_choice == (float)i;
                if(ImGui::Selectable(choices[i], is_selected))
                    render_choice = (float)i;
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        ImGui::End();

        //cam data
        cam.display_data();
        //texture data
        tm.draw_info_windw();

        //ImGui::ShowDebugLogWindow();
        //ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        dt = glfwGetTime() - now;
        while(auto err = glGetError()) std::cout << err << "\n";
        //std::cout << glGetError() << "\n";
    }
    }

    cleanup();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void scene_resize(int width, int height, CameraController& cam, Framebuffer& fbuffer)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    cam.update_proj(width, height);
    fbuffer.resize(width, height);
}