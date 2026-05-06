#include "CameraController.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>

CameraController::CameraController(int screen_w, int screen_h, float fov, const glm::vec3& i_pos)
:   Camera(screen_w, screen_h, fov),
    m_yaw(0),
    m_pitch(0),
    m_movement_speed(5.f),
    m_angle_speed(50.f)
{
    pos = i_pos;
    update_view();
}

void CameraController::update(float dt)
{
    float speed = m_movement_speed*dt;
    float angle_speed = m_angle_speed*dt;

    bool forward = ImGui::IsKeyDown(ImGuiKey_W);
    bool backward = ImGui::IsKeyDown(ImGuiKey_S);

    bool right = ImGui::IsKeyDown(ImGuiKey_D);
    bool left = ImGui::IsKeyDown(ImGuiKey_A);

    int front_back = 0;
    int right_left = 0;

    if(forward) front_back = 1;
    else if(backward) front_back = -1;

    if(right) right_left = 1;
    else if(left) right_left = -1;

    pos += speed * front_back * front;
    pos += speed * right_left * (glm::cross(front, up));

    int yaw_right = ImGui::IsKeyDown(ImGuiKey_RightArrow);
    int yaw_left = ImGui::IsKeyDown(ImGuiKey_LeftArrow);

    int pitch_up = ImGui::IsKeyDown(ImGuiKey_UpArrow);
    int pitch_down = ImGui::IsKeyDown(ImGuiKey_DownArrow);

    int dyaw = 0;
    int dpitch = 0;

    if(yaw_right) dyaw = 1;
    else if(yaw_left) dyaw = -1;

    if(pitch_up) dpitch = 1;
    else if(pitch_down) dpitch = -1;
    
    m_yaw += angle_speed*dyaw;
    m_pitch += angle_speed*dpitch;

    if(m_yaw < -180) m_yaw += 360;
    else if(m_yaw > 180) m_yaw -= 360;

    if(m_pitch < -90) m_pitch = -90;
    else if(m_pitch > 90) m_pitch = 90;



    front.x = glm::cos(glm::radians(m_pitch)) * glm::cos(glm::radians(m_yaw));
    front.z = glm::cos(glm::radians(m_pitch)) * glm::sin(glm::radians(m_yaw));
    front.y = glm::sin(glm::radians(m_pitch));

    update_view();
}

void CameraController::display_data()
{
    ImGui::Begin("Camera");
    ImGui::Separator();

    ImGui::Text("Position");
    ImGui::DragFloat("X", &pos.x, 0.10f, 0.0f, 0.0f, "%.3f");
    ImGui::DragFloat("Y", &pos.y, 0.10f, 0.0f, 0.0f, "%.3f");
    ImGui::DragFloat("Z", &pos.z, 0.10f, 0.0f, 0.0f, "%.3f");
    ImGui::Separator();

    ImGui::Text("Euler angles");
    ImGui::DragFloat("Yaw", &m_yaw, 0.10f, -180.f, 180.f, "%.3f");
    ImGui::DragFloat("Pitch", &m_pitch, 0.10f, -180.f, 180.f, "%.3f");
    ImGui::Separator();

    ImGui::Text("Constants");
    ImGui::DragFloat("Movement speed", &m_movement_speed, 0.10f, 0.f, FLT_MAX);
    ImGui::DragFloat("Turn speed", &m_angle_speed, 0.10f, 0.f, FLT_MAX);

    ImGui::End();
}
