#pragma once

#include "glad/glad.h"

/*

Header file for framebuffer objects:

putting all initialization and resizing code into its class, so our code is a little cleaner

Framebuffer contains:
    - width
    - height
    - FBO, texture, RBO handles
    (all unsigned ints)
*/

class Framebuffer
{
public:
    /*
        @brief Initializes the framebuffer object
        @param W width in pixels
        @param H height in pixels
        @note Attempts to create a framebuffer object and initilaize it with a texture (color buffer)
            and render buffer (depth 24 + stencil 8). The success of initialization can be polled with `check_status()`
    */
    Framebuffer(const unsigned int W, const unsigned int H);

    /*
        @brief Frees memory and handles
    */
    ~Framebuffer();
    /*
        @brief Polls for the status of the framebuffer, should return `GL_FRAMEBUFFER_COMPLETE` if everything went right
        @return GLenum code
    */
    inline GLenum check_status();

    /*
        @brief Resizes the framebuffer
        @param W new width
        @param H new height
    
    */
    void resize(unsigned int W, unsigned int H);

    /*
        @return the width of the framebuffer
    */
    unsigned int get_width();
    /*
        @return the height of the framebuffer
    */
    unsigned int get_height();

    /*
        @return framebuffer's texture's opengl handle
    */
    unsigned int get_texture_handle();

    /*
        @brief Binds the framebuffer
    */
    void bind();

private:
    unsigned int width;
    unsigned int height;
    unsigned int FBO;
    unsigned int texture;
    unsigned int RBO;

};