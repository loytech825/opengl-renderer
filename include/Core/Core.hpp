#pragma once

/*

This file contains basic init / frame begin and frame end functions, that take care of
various internal states of GLFW and ImGui

*/


class GLFWwindow;

namespace Core
{
    /*
        @brief Initializes glfw and imgui
        @param SCR_WIDTH width of window
        @param SCR_HEIGHT height of window
        @param TITLE of the window
        @return pointer to glfw window created, `nullptr` if there are errors
    */
    GLFWwindow* init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, const char* TTITLE);

    /*
        @brief Destroys glfw and imgui contexts
    */
    void shutdown();

    /*
        @brief Initializes the imgui frame and clears the main screen buffer
    */
    void init_frame();

    /*
        @brief Ends the frame and swaps the buffers
        @param window the window on which to swap the buffers
    */
    void end_frame(GLFWwindow* window);
}