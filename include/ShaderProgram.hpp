#include "glad/glad.h"
#include <filesystem>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
    ShaderProgram(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other);
    ShaderProgram& operator=(ShaderProgram&& other);
    ~ShaderProgram();
    void bind();
    void unbind();


    void set_uniform(const std::string& name, float value);
    void set_uniform(const std::string& name, int value);
    void set_uniform(const std::string& name, bool value);
    void set_uniform(const std::string& name, glm::vec3 value);
    void set_uniform(const std::string& name, glm::vec4 value);
    void set_uniform(const std::string& name, glm::mat4 value);
    void set_uniform(const std::string& name, unsigned int count, void* pointer);

private:

    //reads vertex and fragment shader files
    //and compiles both shaders and links them to the program
    void compile_shader(const std::filesystem::path& v_path, const std::filesystem::path& f_path);
    void check_compile_errors(unsigned int shader_id, const std::string& type);
    unsigned int m_ID;

};