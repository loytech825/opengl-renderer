#pragma once
#include "glm/glm.hpp"

//base class for storing camera data such s position, fov...
class Camera
{
public:
    Camera(int screen_w, int screen_h, float fov);

    glm::mat4 get_proj_x_view();

    void update_proj(int screen_w, int screen_h, float fov = 45.f);
    void update_view();

protected:
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;

private:
    glm::mat4 proj;
    glm::mat4 view;
};