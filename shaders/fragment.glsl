#version 330 core
out vec4 FragColor;

in VS_OUT {
   vec3 frag_pos;
   vec2 tex_coords;

   //these are in tangent space coordinates, needed for lighting
   vec3 tangent_light_coords;
   vec3 tangent_camera_pos;
   vec3 tangent_frag_pos; // unused, since we use directional light
   vec3 tangent_light_dir;

   //for visualisation and debug NOT calculations
   mat3 TBN_inv;
} fs_in;

uniform sampler2D u_texture_diffuse0;
uniform sampler2D u_texture_specular0;
uniform sampler2D u_texture_normal0;

uniform float u_choose_render;

//PHONG LIGHTING
vec3 light_color = vec3(1, 1, 1);

float ambient_strength = 0.1;
float diffuse_strength = 1.f;

/*
Lighting is done in tangent space
Current model - Phong
*/

void main()
{    
   vec3 object_color = texture(u_texture_diffuse0, fs_in.tex_coords).xyz;

   //read from texture
   vec3 texture_normal = texture(u_texture_normal0, fs_in.tex_coords).xyz;
   texture_normal =  normalize(2*texture_normal-1);
   //--------------------------------------------------------------------------------

   //ambient
   //base color, so its not completely dark even when facing away from light
   vec3 ambient = ambient_strength * light_color;
   //--------------------------------------------------------------------------------


   //diffuse
   // check light dir against normal 
   vec3 frag_to_light = -fs_in.tangent_light_dir; // usually is light_pos - frag_pos but since the light is directional its just this
   float diff = max(dot(frag_to_light, texture_normal), 0.0);
   vec3 diffuse = diffuse_strength * diff * light_color;
   //--------------------------------------------------------------------------------


   // specular
   // check check camera pos against light reflection 
   vec3 specular_base = texture(u_texture_specular0, fs_in.tex_coords).xyz;
   vec3 frag_to_cam = normalize(fs_in.tangent_camera_pos - fs_in.tangent_frag_pos);

   //reflect need incident vector and so we need the -
   //incident vector = from light to frag
   vec3 reflected_light = reflect(-frag_to_light, texture_normal);

   float spec = pow(max(dot(frag_to_cam, reflected_light), 0.0f), 32);
   vec3 specular = spec * specular_base * light_color;
   //--------------------------------------------------------------------------------
   vec3 result;

   //different rendering modes
   if(u_choose_render == 0.0f)
      result = (ambient + diffuse + specular) * object_color;
   else if (u_choose_render == 1.0f)
      result = fs_in.TBN_inv * texture_normal;
   else if (u_choose_render == 2.0f)
      result = frag_to_cam;
   else if (u_choose_render == 3.0f)
      result = frag_to_light;
   else if(u_choose_render == 4.0f)
      result = reflected_light;
   else if(u_choose_render == 5.0f)
      result = specular_base;

   FragColor = vec4(result, 1);
}