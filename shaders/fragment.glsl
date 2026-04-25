#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_texture_diffuse0;

void main()
{    
    FragColor = texture(u_texture_diffuse0, TexCoords);
    //FragColor = vec4(1, 1, 1, 1);
}