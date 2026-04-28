#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 frag_normal;
in vec3 frag_pos;

uniform sampler2D u_texture_diffuse0;
uniform sampler2D u_texture_specular0;

uniform vec3 u_camera_pos;

uniform float u_choose_render;
//PHONG LIGHTING

vec3 light_dir = normalize(vec3(0, 0, -1));
vec3 light_color = vec3(1, 1, 1);

float ambient_strength = 0.1;
float diffuse_strength = 1.f;

void main()
{    
   vec3 object_color = texture(u_texture_diffuse0, TexCoords).xyz;

   vec3 ambient = ambient_strength * light_color;

   //diffuse
   vec3 frag_to_light = -light_dir; // usually is light_pos - frag_pos but since the light is directional its just this
   float diff = max(dot(frag_to_light, frag_normal), 0.0);
   vec3 diffuse = diffuse_strength * diff * light_color;

   //specular
   vec3 specular_base = texture(u_texture_specular0, TexCoords).xyz;
   vec3 frag_to_cam = normalize(u_camera_pos - frag_pos);
   //reflect need incident vector and so we need the -
   vec3 reflected_light = reflect(-frag_to_light, frag_normal);

   float spec = pow(max(dot(frag_to_cam, reflected_light), 0.0f), 32);
   //spec = 1;
   vec3 specular = spec * specular_base * light_color;

   vec3 result;

   if(u_choose_render == 0.0f)
      result = (ambient + diffuse + specular) * object_color;
   else if (u_choose_render == 1.0f)
      result = frag_normal;
   else if (u_choose_render == 2.0f)
      result = frag_to_cam;
   else if (u_choose_render == 3.0f)
      result = frag_to_light;
   else if(u_choose_render == 4.0f)
      result = reflected_light;
   else if(u_choose_render == 5.0f)
      result = specular * object_color;

   FragColor = vec4(result, 1);
}