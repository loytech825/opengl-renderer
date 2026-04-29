#include "Texture.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>
#include <filesystem>
#include <algorithm>

TextureManager::TextureManager() {}

TextureManager::~TextureManager()
{
    for(auto it = m_loaded_textures.begin(); it != m_loaded_textures.end(); it++)
    {
        glDeleteTextures(1, &it->second.id);
    }
}

Texture TextureManager::load_texture(const std::string &path)
{
    return load_texture(path, DEFAULT);
}

Texture TextureManager::load_texture(const std::string &path, TextureType type)
{
    //everytime we load a texture, this counter increases, so we get a unique id
    static Texture current_text = 0;

    std::cout << "Loading " << path << "...\n";

    TextureData texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    //if the texture is already loaded, return it
    auto it = std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(), [&path](const auto& pair){ return pair.second.path == path.substr(path.find_last_of('/'));}); 
    if(it != m_loaded_textures.end())
    {   
        std::cout << path << " already loaded!\n";
        return it->first;
    }

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(std::filesystem::path(path).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        glDeleteTextures(1, &texture.id);
        texture.id = -1;
    }
    stbi_image_free(data);

    //DEBUG: std::cout << path << ": " << width << "x" << height << ", " << nrChannels << "\n";
    texture.path = path.substr(path.find_last_of('/'));

    m_loaded_textures.emplace(current_text, texture);

    return current_text++;
}

void TextureManager::unload_texture(Texture &texture)
{
    auto it = m_loaded_textures.find(texture);
    //texture not found = not loaded
    if(it == m_loaded_textures.end()) return;

    auto& data = it->second;

    std::cout << "Unloading " << data.path << "!\n";

    glDeleteTextures(1, &data.id);
    texture = -1;
    m_loaded_textures.erase(it);
}

void TextureManager::unload_all()
{
    for(auto& [id, data] : m_loaded_textures)
    {
        glDeleteTextures(1, &data.id);
    }

    m_loaded_textures.clear();
}

bool TextureManager::bind_texture(Texture texture, unsigned int unit)
{
    auto it = m_loaded_textures.find(texture);
    //texture not found = not loaded
    if(it == m_loaded_textures.end()) return false;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, it->second.id);
    return true;
}

//fix err state
TextureData TextureManager::get_data(Texture texture)
{
    auto it = m_loaded_textures.find(texture);
    if(it == m_loaded_textures.end())
    {
        TextureData d;
        d.id = -1;
        return d;
    }
    return it->second;
}
