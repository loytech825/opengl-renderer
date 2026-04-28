#pragma once

#include <string>
#include <unordered_map>

//TODO: unify texture references across textures
// for example unload texture needs to invalidate all references
//    external handle  internal texture ??
//map: unsigned int -> texture          ??

// 2 structs one for interface to opengl and texture metadata
// other to reference texture in program (literally unsigned int)

//used externally
typedef unsigned int Texture;



enum TextureType : unsigned int
{
    DEFAULT, DIFFUSE, SPECULAR, NORMAL, HEIGHT
};

//used by the manager to actually store data
struct TextureData
{
    unsigned int id;
    TextureType type;
    std::string path;
};

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    Texture load_texture(const std::string& path);
    Texture load_texture(const std::string& path, TextureType type);

    //simply unloads the texture and invalidates the passed object
    //we need to update params like id to -1
    void unload_texture(Texture& texture);

    bool bind_texture(Texture texture, unsigned int unit);

    TextureData get_data(Texture texture);

private:
    std::unordered_map<Texture, TextureData> m_loaded_textures;
};