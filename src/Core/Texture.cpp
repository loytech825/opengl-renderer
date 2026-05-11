#include "Texture.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>
#include <filesystem>

#include <algorithm>
#include <imgui.h>

TextureManager::TextureManager()
//:   m_current_texture(0)   
{
    gen_default_texture();
}

TextureManager::~TextureManager()
{
    for(auto it = m_loaded_textures.begin(); it != m_loaded_textures.end(); it++)
    {
        glDeleteTextures(1, &it->id);
    }
}

Texture TextureManager::load_texture(const std::string &path)
{
    return load_texture(path, DEFAULT);
}

Texture TextureManager::load_texture(const std::string &path, TextureType type)
{

    std::cout << "Loading " << path << "...\t" << type << "\n";


    //===================================================================================================================
    //                                      CHECK IF ALREADY LOADED
    //===================================================================================================================
    auto it = std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(), [&path](const auto& data){ return data.path == path;/*.substr(path.find_last_of('/'));*/});
    if(it != m_loaded_textures.end())
    {   
        //std::cout << path << " already loaded!" << it->id << "\n";
        //index
        return (it-m_loaded_textures.begin());
    }

    TextureData texture;
    //===================================================================================================================
    //                                  CREATING TEXTURE + TEXTURE PARAMS
    //===================================================================================================================
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    int width, height, nrChannels;

    //===================================================================================================================
    //                                          IMAGE LOADING
    //===================================================================================================================
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    
    unsigned char *data = stbi_load(std::filesystem::path(path).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        //we need to change our texture to SRGB if out texture is image data
        GLint internal_format = (type==DIFFUSE) ? GL_SRGB : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        texture.W = width;
        texture.H = height; 
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
    //===================================================================================================================
    //                                      REGISTERING IN HASHMAP
    //===================================================================================================================
    texture.path = path;//path.substr(path.find_last_of('/'));
    texture.type = type;

    m_loaded_textures.emplace_back(texture);

    return m_loaded_textures.size()-1;
}

Cubemap TextureManager::load_cubemap(const std::vector<std::string>& paths)
{
    //===================================================================================================================
    //                                      CHECK IF ALREADY LOADED -- indexed by first texture
    //===================================================================================================================
    auto it = std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(), [&paths](const auto& data){ return data.path == paths[0];}); 
    if(it != m_loaded_textures.end())
    {   
        //std::cout << path << " already loaded!\n";
        //index
        return (it-m_loaded_textures.begin());
    }


    //===================================================================================================================
    //                                      CREATING CUBEMAP + PARAMS
    //===================================================================================================================
    TextureData cubemap;
    glGenTextures(1, &cubemap.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);

    //===================================================================================================================
    //                                          LOADING TEXTURES
    //===================================================================================================================
    int width, height, nr_channels;

    for(unsigned int i = 0; i < paths.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nr_channels, 0);

        if(data)
        {
            //image needs to go to srgb
            //SRGB effectively does pixel^(gamma)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "failed loading cubemap texture at " << paths[i] << "!\n";
        }
        stbi_image_free(data);
    }
    //===================================================================================================================
    //                                      TEXTURE WRAPPING PARAMS
    //===================================================================================================================
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubemap.path = paths[0];
    cubemap.type =  CUBEMAP;

    m_loaded_textures.emplace_back(cubemap);

    return m_loaded_textures.size()-1;
}

void TextureManager::unload_texture(Texture &texture)
{
    //texture id out of bounds
    if((unsigned int)texture >= m_loaded_textures.size() || (unsigned int)texture < 0) return;

    TextureData data = m_loaded_textures[texture];
    
    if(data.id == 0) return;

    std::cout << "Unloading " << data.path << "!\n";

    glDeleteTextures(1, &data.id);

    //set this texture to default in case anything else needed it
    m_loaded_textures[texture] = m_loaded_textures[0];
    texture = -1;
}

void TextureManager::unload_all()
{
    for(auto& data : m_loaded_textures)
    {
        glDeleteTextures(1, &data.id);
    }

    m_loaded_textures.clear();
    //m_current_texture = 0;
    gen_default_texture();
}

bool TextureManager::bind_texture(Texture texture, unsigned int unit)
{
    //if texture not found, bind default texture
    if((unsigned int)texture >= m_loaded_textures.size() || (unsigned int)texture < 0)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_loaded_textures[0].id);
        //std::cout << "Texture invalid! "  << texture << "\n";
        return false;
    };

    TextureData data = m_loaded_textures[texture];

    //NORMAL TEXTURE BIND
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, data.id);
    return true;
}

bool TextureManager::bind_cubemap(Cubemap cubemap, unsigned int unit)
{
    //if texture not found, bind default texture
    if((unsigned int)cubemap >= m_loaded_textures.size() || (unsigned int)cubemap < 0)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_loaded_textures[0].id);
        //std::cout << "Texture invalid! "  << texture << "\n";
        return false;
    };

    TextureData data = m_loaded_textures[cubemap];

    //CUBEMAP BIND
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, data.id);
    return true;
}

// also return err texture instead of unusable
TextureData TextureManager::get_data(Texture texture)
{
    if((unsigned int)texture >= m_loaded_textures.size() || (unsigned int)texture < 0)
        return m_loaded_textures[0];

    TextureData data = m_loaded_textures[texture];
    return data;
}

void TextureManager::draw_info_window() 
{
    ImGui::Begin("Textures");
    if(ImGui::TreeNode("Textures loaded"))
    {
        ImGui::BeginChild("TextureList", {0, 0}, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
        for(const auto& data : m_loaded_textures)
        {
            std::string text = std::to_string(data.id) + "(" + std::to_string(data.W) + "x" 
                            + std::to_string(data.H) + "): " + data.path;
            ImGui::Text("%s", text.c_str());
        }
        ImGui::EndChild();

        ImGui::TreePop();
    }
    ImGui::End();
}

//===================================================================================================================
//                                              PRIVATE
//===================================================================================================================
void TextureManager::gen_default_texture()
{

    TextureData default_texture;

    //===================================================================================================================
    //                                  CREATING TEXTURE + TEXTURE PARAMS
    //===================================================================================================================
    glGenTextures(1, &default_texture.id);
    glBindTexture(GL_TEXTURE_2D, default_texture.id);
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //color pixel data of default texture
    unsigned char pixels[12] = { 255,0,0, 0,255,0, 255,0,0, 0,255,0};

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    default_texture.path = "DEFAULT TEXTURE";
    default_texture.W = 2;
    default_texture.H = 2;
    default_texture.type = DEFAULT;

    m_loaded_textures.emplace_back(default_texture);
    //m_current_texture++;
}
