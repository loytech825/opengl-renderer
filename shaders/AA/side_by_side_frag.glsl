#version 330

out vec4 FragColor;
in vec4 gl_FragCoord;

uniform sampler2D u_frame0;
uniform sampler2D u_frame1;

uniform int u_middle;

in vec2 tex_coords;

void main()
{
    if(gl_FragCoord.x < u_middle)
    {
        FragColor = texture(u_frame0, tex_coords);
    }else if(gl_FragCoord.x > u_middle)
    {
        FragColor = texture(u_frame1, tex_coords);
    }

    //draw middle line
    if(gl_FragCoord.x > u_middle-1 && gl_FragCoord.x < u_middle+1)
        FragColor = vec4(0, 1, 0, 1);
}