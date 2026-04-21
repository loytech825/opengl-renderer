#include "ShaderProgram.hpp"
#include <fstream>
#include <iostream>

ShaderProgram::ShaderProgram(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
{
    compile_shader(vertex_path, fragment_path);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
    this->m_ID = other.m_ID;
    other.m_ID = 0;
    return *this;
}

ShaderProgram::~ShaderProgram()
{
    if(m_ID) {glDeleteProgram(m_ID);}
}

void ShaderProgram::bind()
{
    glUseProgram(m_ID);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

void ShaderProgram::set_uniform(const std::string& name, float value)
{glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);}

void ShaderProgram::set_uniform(const std::string& name, int value)
{glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);}

void ShaderProgram::set_uniform(const std::string& name, bool value)
{glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);}

void ShaderProgram::set_uniform(const std::string& name, glm::mat4 value)
{glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);}

void ShaderProgram::set_uniform(const std::string& name, glm::vec3 value)
{glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);} 

void ShaderProgram::set_uniform(const std::string& name, glm::vec4 value)
{glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);} 

void ShaderProgram::set_uniform(const std::string& name, unsigned int count, void* pointer)
{glUniform1iv(glGetUniformLocation(m_ID, name.c_str()), count, (const GLint*)pointer);}



void ShaderProgram::compile_shader(const std::filesystem::path& v_path, const std::filesystem::path& f_path)
{
    std::string v_shader_code;
    std::string f_shader_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    try
    {
        v_shader_file.open(v_path);
        f_shader_file.open(f_path);

        std::stringstream v_ss;
        std::stringstream f_ss;
        v_ss << v_shader_file.rdbuf();
        f_ss << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        v_shader_code = v_ss.str();
        f_shader_code = f_ss.str();
    }
    catch(const std::exception& e)
    {
        std::cerr << "OPENGL_SUPPORT_ERR::SHADER::FILE_FAILED_TO_READ: " << e.what() << '\n';
    }
    
    const char* v_code_raw = v_shader_code.c_str();
    const char* f_code_raw = f_shader_code.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex, 1, &v_code_raw, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    glShaderSource(fragment, 1, &f_code_raw, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    check_compile_errors(m_ID, "PROGRAM");

    //linked to program, safe to delete
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void ShaderProgram::check_compile_errors(unsigned int shader_id, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
            std::cout << "OPENGL_SUPPORT_ERR::SHADER::COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_id, 1024, NULL, infoLog);
            std::cout << "OPENGL_SUPPORT_ERR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}