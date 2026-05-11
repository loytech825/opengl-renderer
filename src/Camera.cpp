#include "Camera.hpp"
#include  "glm/gtc/matrix_transform.hpp"

Camera::Camera(int screen_w, int screen_h, float fov)
:   pos(0,0,0),
    up(0,1,0),
    front(1, 0, 0)
{
    update_proj(screen_w, screen_h, fov);
}

/*void Camera::set_pos(const glm::vec3 &new_pos)
{
    pos = new_pos;
    update_view();
}

void Camera::set_front(const glm::vec3 &new_front)
{
    front = new_front;
    update_view();
}*/

glm::mat4 Camera::get_proj_x_view()
{
    return proj * view;
}

glm::mat4 Camera::get_proj_x_view_no_translation()
{
    return proj * glm::mat4(glm::mat3(view));
}

void Camera::update_proj(int screen_w, int screen_h, float fov)
{
    float ratio = (float)screen_w/(float)screen_h;
    proj = glm::perspective(glm::radians(fov), ratio, (float)1, (float)100);
}

void Camera::update_view()
{
    view = glm::lookAt(pos, pos+front, up);
}

glm::vec3 Camera::get_pos()
{
    return pos;
}
