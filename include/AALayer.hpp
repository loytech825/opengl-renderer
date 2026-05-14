#pragma once
#include "AppLayer.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "CameraController.hpp"
#include "Framebuffer.hpp"

class AALayer : public Core::AppLayer
{
public:
    AALayer(unsigned int W, unsigned int H);

    void on_update(float dt) override;
    void on_render() override;

    bool is_running() override;

private:
    unsigned int m_VAO, m_VBO, m_post_VAO, m_post_VBO;
    ShaderProgram m_base_shader;
    ShaderProgram m_side_by_side_shader;

    Framebuffer m_msaa_fb;
    //to convert msaa to regular texture
    Framebuffer m_intermediate_fb;
    //to render the scene regularly
    Framebuffer m_regular_fb;
    Framebuffer m_output_fb;

    bool scene_focused;

    TextureManager m_texture_manager;

    CameraController m_cam;

    void resize(const unsigned int W, const unsigned int H);
};