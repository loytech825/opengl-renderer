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
    Model(const std::string& path);
    void Draw(ShaderProgram& shader);
    
    bool gamma_correction;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

private:

    //recursive model traversal and parsing
    void load_model(const std::string& path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
};