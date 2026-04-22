#pragma once
#include "Camera.hpp"


#include <glm/glm.hpp> 
class GLFWwindow;

// camera wrapper
// added functionality such as movement...
class CameraController : public Camera
{
public:
    /*
    @param screen_w - in pixels
    @param screen_h - in pixels
    @param fov - in degrees
    */
    CameraController(int screen_w, int screen_h, float fov, const glm::vec3& pos = {0, 0, 0});
    //movement code...
    void update(float dt, GLFWwindow* window);

private:

    //eulerian angles for calculating where camera is looking
    float yaw;
    float pitch;
    float roll;

};