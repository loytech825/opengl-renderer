#pragma once

#include <string>
#include <vector>
#include <unordered_map>

//TODO: unify texture references across textures
// for example unload texture needs to invalidate all references
//    external handle  internal texture ??
//map: unsigned int -> texture          ??

//TODO: separate cubemap and texture for faster binding?

// 2 structs one for interface to opengl and texture metadata
// other to reference texture in program (literally unsigned int)

//used externally
//since they are bouth uints we can use them to index the same array
typedef unsigned int Texture;
typedef unsigned int Cubemap;


// Defines for various types of texture types, for easier checking and correctly binding different types
enum TextureType : unsigned int
{
    DEFAULT, DIFFUSE, SPECULAR, NORMAL, HEIGHT, CUBEMAP
};

//used by the manager to actually store data
struct TextureData
{
    unsigned int id;
    TextureType type;
    std::string path;

    //dont think we need this but good to have
    unsigned int W, H;
};

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    /*
        @brief Loads a texture with a default type
        @param path the path to the image file
        @return a texture
    */ 
    Texture load_texture(const std::string& path);
    /*
        @brief Loads a texture with provided type
        @param path the path to the image file
        @param type the type of the image (diffuse/normal...)
        @return a texture
    */ 
    Texture load_texture(const std::string& path, TextureType type);

    /*
        @brief Loads A cubemap with textures provided
        @param paths to the 6 textures of a cubemap in order: `right`, `left`, `top`, `bottom`, `front`, `back`
        @return Cubemap loaded reference to cubemap
    */
    Cubemap load_cubemap(const std::vector<std::string>& paths);

    //simply unloads the texture and invalidates the passed object
    //we need to update params like id to -1
    void unload_texture(Texture& texture);

    /*
        @brief Unloads all textures
    */
    void unload_all();

    /*
        @brief Binds a texture to a unit
        @param texture to bind
        @param unit to bind the texture to
        @return `true` if bind successful, `false` otherwise
    */
    bool bind_texture(Texture texture, unsigned int unit);

    /*
        @brief Binds a cubemap to a unit
        @param cubemap to bind
        @param unit to bind the texture to
        @return `true` if bind successful, `false` otherwise
    */
    bool bind_cubemap(Cubemap cubemap, unsigned int unit);

    /*
        @brief Get more detailed texture information
        @param texture to poll
        @return texture metadata
    */
    TextureData get_data(Texture texture);

    /*
        @brief Renders an imgui window providing information regarding textures
    */
    void draw_info_window();

private:

    /*
        @brief Generates a placeholder texture for invalid calls,
        the generated texture should always be at `m_loaded_textures[0]`
    */
    void gen_default_texture();

    //since we increase texture ids linearly we can use a vector
    std::vector<TextureData> m_loaded_textures;

    //holds the next free id
    //unsigned int m_current_texture;
};