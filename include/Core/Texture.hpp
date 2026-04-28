#pragma once

#include <string>
#include <unordered_map>

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    Texture load_texture(const std::string& path);
    Texture load_texture(const std::string& path, const std::string& type);

    //simply unloads the texture and invalidates the passed object
    //we need to update params like id to -1
    void unload_texture(Texture& texture);

    bool bind_texture(const Texture& texture, unsigned int unit);

private:
    std::unordered_map<std::string, Texture> m_loaded_textures;
};