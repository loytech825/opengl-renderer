#include "AALayer.hpp"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

AALayer::AALayer(unsigned int W, unsigned int H)

:   m_base_shader("shaders/AA/vertex.glsl", "shaders/AA/fragment.glsl"),
    m_cam(W, H, 45),
    m_msaa_fb(W, H, 4),
    m_output_fb(W, H)
{

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void AALayer::on_update(float dt)
{
    if(scene_focused)
        m_cam.update(dt);
}

void AALayer::on_render()
{
    //==============================================================
    //              DRAWING TO MXAA FRAMEBUFFER
    //==============================================================
    m_msaa_fb.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_base_shader.bind();
    m_base_shader.set_uniform("u_proj_view", m_cam.get_proj_x_view());
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindVertexArray(0);


    //==============================================================
    //      BLIT TO OUTPUT BUFFER
    //==============================================================
    m_msaa_fb.bind_as_read();
    unsigned int msaa_W = m_msaa_fb.get_width();
    unsigned int msaa_H = m_msaa_fb.get_height();
    m_output_fb.bind_as_draw();
    unsigned int out_W = m_output_fb.get_width();
    unsigned int out_H = m_output_fb.get_height();
    glBlitFramebuffer(0, 0, msaa_W, msaa_H, 0, 0, out_W, out_H, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //==============================================================
    //             RENDER TO IMGUI WINDOW
    //==============================================================
    ImGui::SetNextWindowSize({(float)out_W, (float)out_H}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene");

    ImVec2 new_scene_size = ImGui::GetContentRegionAvail();

    if(!(new_scene_size.x == out_W && new_scene_size.y == out_H))
    {
        resize(new_scene_size.x, new_scene_size.y);
    }

    ImGui::Image((ImTextureRef)m_output_fb.get_texture_handle(), new_scene_size, {0, 1}, {1, 0});

    scene_focused = ImGui::IsWindowFocused();

    ImGui::End();

    m_cam.display_data();
}

bool AALayer::is_running() {return true;}

void AALayer::resize(const unsigned int W, const unsigned int H)
{
    glViewport(0, 0, W, H);
    m_cam.update_proj(W, H);
    m_msaa_fb.resize(W, H);
    m_output_fb.resize(W, H);
}
