#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;


/*
Lighting calculations will be done in tangent space (normal map is provided in tangent space)

*/

out VS_OUT {
   vec3 frag_pos;
   vec2 tex_coords;

   //these are in tangent space coordinates, needed for lighting
   vec3 tangent_light_coords;
   vec3 tangent_camera_pos;
   vec3 tangent_frag_pos; // unused, since we use directional light
   vec3 tangent_light_dir;

   //for visualisation and debug NOT calculations
   mat3 TBN_inv;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_normal_matrix;
uniform mat4 u_proj_view;

uniform vec3 u_camera_pos;
uniform vec3 u_light_dir;

void main()
{

   vs_out.tex_coords = aTexCoords;
   vs_out.frag_pos = aPos;

   //tangent + normal is given to us by model loader, we just need to transform them according to the model transformation
   vec3 T = normalize(vec3(u_normal_matrix * vec4(aTangent, 0.0)));
   vec3 N = normalize(vec3(u_model * vec4(aNormal, 0.0)));

   //Gram - Schmidt process for orthogonalizing vectors
   T = normalize(T - dot(T, N) * N);
   vec3 B = cross(N, T);

   // change of basis matrix (world -> TBN)
   //TBN is our tangent space, in which normal vectors are defined
   mat3 TBN = mat3(T, B, N);
   // change of basis (TBN -> world) (since TBN is orthonormal, inverse = transpose )
   mat3 TBN_inv = transpose(TBN);

   //forward to fragment
   vs_out.tangent_camera_pos = TBN_inv * u_camera_pos;
   vs_out.tangent_frag_pos = TBN_inv * vs_out.frag_pos;
   vs_out.tangent_light_dir = TBN_inv * u_light_dir;
   vs_out.TBN_inv = TBN_inv;

   gl_Position = u_proj_view * u_model * vec4(aPos, 1.0);
}