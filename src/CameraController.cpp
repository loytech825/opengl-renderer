#include "CameraController.hpp"
#include <GLFW/glfw3.h>

CameraController::CameraController(int screen_w, int screen_h, float fov, const glm::vec3& i_pos)
:   Camera(screen_w, screen_h, fov)
{
    pos = i_pos;
    update_view();
}

void CameraController::update(float dt, GLFWwindow *window)
{
    float speed = 5.f*dt;

    int forward = glfwGetKey(window, GLFW_KEY_W);
    int backward = glfwGetKey(window, GLFW_KEY_S);

    int right = glfwGetKey(window, GLFW_KEY_D);
    int left = glfwGetKey(window, GLFW_KEY_A);

    int front_back = 0;
    int right_left = 0;

    if(forward == GLFW_PRESS) front_back = 1;
    else if(backward == GLFW_PRESS) front_back = -1;

    if(right == GLFW_PRESS) right_left = 1;
    else if(left == GLFW_PRESS) right_left = -1;

    pos += speed * front_back * front;
    pos += speed * right_left * (glm::cross(front, up));

    update_view();
}
