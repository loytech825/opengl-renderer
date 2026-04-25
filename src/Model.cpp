#include "Model.hpp"
#include "Core/Texture.hpp"
#include <iostream>

Model::Model(const std::string &path)
{
    load_model(path);
}

void Model::Draw(ShaderProgram &shader)
{
    for(auto& mesh : meshes)
    {
        mesh.Draw(shader);
    }
}

void Model::load_model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
                                                | aiProcess_GenSmoothNormals
                /*dont know if we need this*/   | aiProcess_FlipUVs
                /*if we already flip in texture*/
                /*loading*/
                                                | aiProcess_CalcTangentSpace
                                                | aiProcess_JoinIdenticalVertices   );

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
        return;
    }
    
    //retrieve the directory of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    //recursively process nodes
    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        //all meshes stored in the scene, node only holds
        // "pointers" aka indices
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }
    //after meshes, we recursively process child nodes
    for(int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }

}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    //vectors to populate
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //avoid unnecessary allocations
    vertices.reserve(mesh->mNumVertices);

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; //intermediate variable to convert from assimp to glm

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.position = vector;

        if(mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if(mesh->mTextureCoords[0]) // check if the mesh has coords
        {
            vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;

            //tangents
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.z = mesh->mTangents[i].y;
            vertex.tangent.y = mesh->mTangents[i].z;

            //bitangent
            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }else
        {
            vertex.tex_coords = glm::vec2{0.f, 0.f};
        }

        vertices.push_back(vertex);
    }

    //cant preallocate vector since num of indices is unknown
    //check faces add indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //materials

    //each mesh has a single material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    //need a naming convention for variables in shader so
    //we can set them dynamically
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

    // 1. diffuse mapss
    std::vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    // 2. specular maps
    std::vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    // 3. normal maps
    std::vector<Texture> normal_maps = load_material_textures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    // 4. height maps
    std::vector<Texture> height_maps = load_material_textures(material, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), height_maps.begin(), height_maps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name)
{
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        //check if texture was loaded before
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        //if texture not yet loaded
        if(!skip)
        {
            //TODO: figure out a texutre struct layout 
            Texture texture;
            texture.id = load_texture(directory + "/" + str.C_Str());
            texture.type = type_name;
            texture.path = str.C_Str();
            //TODO: figure out a way to link these (data duplication)
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}
