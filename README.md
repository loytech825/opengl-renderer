# OPENGL renderer

## Texture Management:
Texture manager class handles all the TextureData. Each texture has:
    - its opengl handle
    - path where it's stored
    - texture type (diffuse, normal, etc.)
When loading a texture, external "users" of textures get a Texture object (unsigned int), which points to an internal TextureData object. Each Texture is a unique number. Calling TextureManager::unload_all() unloads all textures and deletes their opengl storage. It does not reset the Texture counter, so if there are any Texture objects pointing to deleted TextureData, we can handle this with a default texture. This can be used between scenes to reset internal storage. When binding a texture, you call TextureManager::bind_texture() with a Texture and a bind location. 