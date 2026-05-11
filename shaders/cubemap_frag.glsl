#version 330

out vec4 FragColor;

in vec3 tex_coords;

uniform samplerCube u_cubemap_texture;

void main()
{
    FragColor = texture(u_cubemap_texture, tex_coords);
}