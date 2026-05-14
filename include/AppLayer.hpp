#pragma once

/*

Base class for application

Made so we can separate app init, loop, and destroy from framework stuff like glfw and imgui

*/

namespace Core{

class AppLayer
{

public:
    AppLayer() {};
    virtual ~AppLayer() {};

    virtual void on_update(float dt) = 0;
    virtual void on_render() = 0;

    virtual bool is_running() = 0;
};
}