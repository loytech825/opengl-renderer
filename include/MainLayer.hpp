#pragma once
#include "AppLayer.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "CameraController.hpp"
#include "Framebuffer.hpp"
#include "Model.hpp"

class GLFWwindow;

class MainLayer : public Core::AppLayer
{
public:
    MainLayer(unsigned int w, unsigned int h);
    ~MainLayer() {};

    virtual void on_update(float dt) override;
    virtual void on_render() override;

    bool is_running() {return m_running;}

private:

    void resize(unsigned int w, unsigned int h);

    ShaderProgram m_shader;
    ShaderProgram m_post_process;

    TextureManager m_texture_manager;

    Framebuffer m_framebuffer;
    //before post process
    Framebuffer m_intermediate;
    CameraController m_cam;

    bool m_running;


    Model m_backpack;

    int m_base_render_choice;
    int m_post_render_choice;
    glm::vec3 light_dir;

    bool scene_focused;

    //post process
    unsigned int VBO;
    unsigned int VAO;
};