#pragma once
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "ShaderProgram.hpp"


//TODO write decostructor
/*

CURRENTLY THIS FILE IS MORE OR LESS COPY/PASTE FROM learnopengl.com

*/

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    //bone indexes which will influence this vertex
    //int m_boneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    //float m_weights[MAX_BONE_INFLUENCE];
};

//this will 100% need a rewrite
struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

/*

    Primitive "Model" type, stores its textures, vertices and indices
    Each mesh requires its own draw call

*/
class Mesh
{
public:

    Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& ind, const std::vector<Texture>& text);
    ~Mesh();
    void Draw(ShaderProgram& shader);


    //maybe would be good to store vertices of all meshes contiguously
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<Texture>        textures;

    unsigned int VAO;

private:
    void setup_mesh();

    unsigned int VBO, EBO;
};