#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 u_proj_view;

out vec3 tex_coords;


void main()
{
    tex_coords = aPos;
    vec4 pos = u_proj_view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}