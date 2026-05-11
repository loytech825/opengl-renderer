#version 330 core
out vec4 FragColor;

in vec4 gl_FragCoord;

in vec2 tex_coords;

uniform sampler2D u_frame;
uniform int u_post_render_choice;

const float GAMMA = 2.2f;

vec4 get_color() { return texture(u_frame, tex_coords); }

vec4 inverse_color() { return vec4(vec3(1.0-get_color()), 1.0); }

vec4 grayscale_average()
{
    float avg = (get_color().x + get_color().y + get_color().z) / 3;
    return vec4(avg, avg, avg, 1);
}

vec4 grayscale_corrected()
{
    float avg = 0.2126 * get_color().r + 0.7152 * get_color().g + 0.0722 * get_color().b;
    return vec4(avg, avg, avg, 1);
}

vec4 kernel_effect()
{
    //KERNEL EFFECT DEFINES

    const float offset = 1.0/300.0;

    vec2 offsets[9] = vec2[](

        vec2(-1.0f,     1.0f),
        vec2(0.0f,      1.0f),    //top row
        vec2(1.0f,      1.0f),

        vec2(-1.0f,     0.0f),
        vec2(0.0f,      0.0f),      //center row
        vec2(1.0f,      0.0f),  

        vec2(-1.0f,     -1.0f),
        vec2(0.0f,      -1.0f),   //bottom row
        vec2(1.0f,      -1.0f)
    );

    //blur kernel
    float blur[9] = float[](
        1.0/16.0, 2.0/16.0, 1.0/16.0,
        2.0/16.0, 4.0/16.0, 2.0/16.0,
        1.0/16.0, 2.0/16.0, 1.0/16.0
    );

    //edge detection
    float edge[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    //END KERNEL DEFINES

    vec3 color = vec3(0, 0, 0);

    for(int i = 0; i < 9; i++)
    {
        //for some reason new_coords = tex_coords
        vec2 new_coords = offset * offsets[i] + tex_coords;
        vec3 color_to_add = vec3(texture(u_frame, new_coords)); 
        color += color_to_add * edge[i];
    }

    return vec4(color, 1.0);
}

void main()
{
    vec4 color = vec4(0);

    if(u_post_render_choice == 0)
    {
        color = get_color();
    }
    else if(u_post_render_choice == 1)
    {
        color = inverse_color();
    }
    else if(u_post_render_choice == 2)
    {
        color = grayscale_average();
    }
    else if(u_post_render_choice == 3)
    {
        color = grayscale_corrected();
    }
    else if(u_post_render_choice == 4)
    {
        color = kernel_effect();
    }


    //gamma correction
    vec4 gamma_corrected = vec4(pow(vec3(color), vec3(1.0f/GAMMA)), 1.0);

    FragColor = gamma_corrected;
    //vec4 raw = get_color(); 
    //FragColor = (gl_FragCoord.y>200) ? gamma_corrected : raw;//vec4(color, 1.0f);//
}