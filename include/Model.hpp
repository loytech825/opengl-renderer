#pragma once
#include <string>
#include "Core/ShaderProgram.hpp"
#include "Mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class Model
{
public:
    Model(const std::string& path, TextureManager& tm);
    ~Model();
    void Draw(ShaderProgram& shader);
    
    bool gamma_correction;
    std::vector<Mesh> meshes;

    //this is currently only needed to unload textures
    std::vector<Texture> textures_loaded;
    std::string directory;

private:

    //should this be done? having all vertices at the same place helps memory bu logically its fine as is 
    //this will hold all vertices for its meshes for a nicer memory layout
    //std::vector<Vertex> m_vertices;

    //recursive model traversal and parsing
    void load_model(const std::string& path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
    TextureManager& m_texture_manager;
};