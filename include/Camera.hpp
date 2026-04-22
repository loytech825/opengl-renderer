#pragma once
#include "glm/glm.hpp"


class Camera
{
public:
    Camera(int screen_w, int screen_h, float fov);


    void set_pos(const glm::vec3& new_pos);
    void set_front(const glm::vec3& new_front);


    glm::mat4 get_proj_x_view();

    void update_proj(int screen_w, int screen_h, float fov = 45.f);
    void update_view();

    glm::vec3 pos;
private:
    
    glm::vec3 front;
    glm::vec3 up;

    glm::mat4 proj;
    glm::mat4 view;
};