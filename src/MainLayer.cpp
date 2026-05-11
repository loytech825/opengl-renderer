#include "MainLayer.hpp"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

MainLayer::MainLayer(unsigned int w, unsigned int h)
:   m_framebuffer(w, h),
    m_intermediate(w, h),
    m_cam(w, h, 45),
    m_shader("shaders/vertex.glsl", "shaders/fragment.glsl"),
    m_post_process_shader("shaders/post_process_vert.glsl", "shaders/post_process_frag.glsl"),
    m_cubemap_shader("shaders/cubemap_vert.glsl", "shaders/cubemap_frag.glsl"),
    m_backpack("res/models/backpack/backpack.obj", m_texture_manager),
    light_dir(0, 0, -1),
    scene_focused(false),
    m_running(true),
    m_base_render_choice(0),
    m_post_render_choice(0)
{

    //post process setup
    float data[]  {
        //pos   uv
        -1, -1, 0, 0,
        -1, 1,  0, 1,
        1, 1,   1, 1,

        1, 1,   1, 1,
        1, -1,  1, 0,
        -1, -1, 0, 0,

    };

    glGenVertexArrays(1, &m_post_VAO);
    glGenBuffers(1, &m_post_VBO);

    glBindVertexArray(m_post_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_post_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_post_process_shader.bind();
    m_post_process_shader.set_uniform("u_frame", 0);
    glUseProgram(0);

    //cubemap setup

    //cube centered @ origin
    constexpr float CUBE_SIZE = 10.0f;
    float cube_vertices[] = {-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,

        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,

        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,

        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,

        -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE,
        -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE,

        -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
        -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE,
        CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE
    };

    glGenVertexArrays(1, &m_cubemap_VAO);
    glGenBuffers(1, &m_cubemap_VBO);

    glBindVertexArray(m_cubemap_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubemap_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_cubemap_shader.bind();
    m_cubemap_shader.set_uniform("u_cubemap_texture", 0);

    //cubemap texture
    m_cubemap = m_texture_manager.load_cubemap({"res/cubemaps/skybox/right.jpg", "res/cubemaps/skybox/left.jpg", "res/cubemaps/skybox/top.jpg", "res/cubemaps/skybox/bottom.jpg", "res/cubemaps/skybox/front.jpg", "res/cubemaps/skybox/back.jpg"});

}

void MainLayer::on_update(float dt)
{
    if(scene_focused) 
        m_cam.update(dt);
}

void MainLayer::on_render()
{

    ImVec2 framebuffer_size{(float)m_framebuffer.get_width(), (float)m_framebuffer.get_height()};


    //ImGui::ShowDemoWindow();
    //ImGui::ShowStyleSelector();

    ImGui::Begin("Render settings");
    ImGui::DragFloat3("Light dir", glm::value_ptr(light_dir), 0.01, -1, 1);

    //=====================================================================================================================
    //                                          BASE IMAGE OPTIONS
    //=====================================================================================================================
    ImGui::SeparatorText("Base image");
    const char* choices[] = {"Normal render", "Normals", "Frag to cam", "Frag to light", "Reflection", "Specular"};
    if(ImGui::BeginCombo("##base_render", choices[m_base_render_choice])){

        for(int i = 0; i < 6; i++)
        {
            bool is_selected = (m_base_render_choice == i);
            if(ImGui::Selectable(choices[i], is_selected))
                m_base_render_choice = i;
            if(is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    //=====================================================================================================================
    //                                          POST PROCESS OPTIONS
    //=====================================================================================================================
    ImGui::SeparatorText("Post process");
    const char* post_choices[] = {"None", "Inverse", "Grayscale Average", "Grayscale Corrected", "Kernel Effect"};
    if(ImGui::BeginCombo("##post_render", post_choices[m_post_render_choice]))
    {
    for(int i = 0; i < 5; i++)
        {
            bool is_selected = (m_post_render_choice == i);
            if(ImGui::Selectable(post_choices[i], is_selected))
                m_post_render_choice = i;
            if(is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();  
    }
    ImGui::End();

    //cam data
    m_cam.display_data();
    //texture data
    m_texture_manager.draw_info_window();


    //=====================================================================================================================
    //                                              MAIN SCENE RENDER
    //=====================================================================================================================

    m_intermediate.bind();
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    m_shader.bind();
    m_shader.set_uniform("u_model", model);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(model));
    m_shader.set_uniform("u_normal_matrix", normal_matrix);   

    m_shader.set_uniform("u_proj_view", m_cam.get_proj_x_view());
    m_shader.set_uniform("u_camera_pos", m_cam.get_pos());
    m_shader.set_uniform("u_choose_render", m_base_render_choice);
    glm::vec3 light_dir_n = glm::normalize(light_dir);
    m_shader.set_uniform("u_light_dir", light_dir_n);
    m_backpack.Draw(m_shader);

    //=====================================================================================================================
    //                                              CUBEMAP RENDER
    //=====================================================================================================================
    glDepthFunc(GL_LEQUAL);
    m_cubemap_shader.bind();
    m_cubemap_shader.set_uniform("u_proj_view", m_cam.get_proj_x_view_no_translation());

    m_texture_manager.bind_cubemap(m_cubemap, 0);
    glBindVertexArray(m_cubemap_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //=====================================================================================================================
    //                                               POST PROCESS
    //=====================================================================================================================

    m_framebuffer.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    m_post_process_shader.bind();
    m_post_process_shader.set_uniform("u_post_render_choice", m_post_render_choice);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_intermediate.get_texture_handle());

    glBindVertexArray(m_post_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //=====================================================================================================================
    //                                      MAIN SCENE IMGUI WINDOW RENDER
    //=====================================================================================================================
    ImGui::SetNextWindowSize(framebuffer_size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene");

    ImVec2 new_scene_size = ImGui::GetContentRegionAvail();

    if(!(new_scene_size.x == framebuffer_size.x && new_scene_size.y == framebuffer_size.y))
    {
        resize(new_scene_size.x, new_scene_size.y);
    }

    ImGui::Image((ImTextureRef)m_framebuffer.get_texture_handle(), new_scene_size, {0, 1}, {1, 0});

    scene_focused = ImGui::IsWindowFocused();

    ImGui::End();
}

void MainLayer::resize(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    m_cam.update_proj(w, h);
    m_framebuffer.resize(w, h);
    m_intermediate.resize(w, h);
}
