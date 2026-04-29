#include "Mesh.hpp"
#include <glad/glad.h>
#include <iostream>


Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& ind, const std::vector<Texture>& text)
:   vertices(vert),
    indices(ind),
    textures(text)
{
    setup_mesh();
}

Mesh::~Mesh()
{
    //TODO before enabling do move/copy semantics
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    //glDeleteVertexArrays(1, &VAO);
}

void Mesh::Draw(ShaderProgram& shader, TextureManager& tm)
{
    unsigned int diffuse_nr = 0;
    unsigned int specular_nr = 0;
    unsigned int normal_nr = 0;
    unsigned int height_nr = 0;

    shader.bind();
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        
        //std::cout << textures[i] << ": " << tm.get_data(textures[i]).id << ", type: " << tm.get_data(textures[i]).type << ", " << tm.get_data(textures[i]).path << "\n";

        std::string location;
        TextureType type = tm.get_data(textures[i]).type;

        if(type == DIFFUSE) location = "u_texture_diffuse" + std::to_string(diffuse_nr++);
        else if(type == SPECULAR) location = "u_texture_specular" + std::to_string(specular_nr++);
        else if(type == NORMAL) location = "u_texture_normal" + std::to_string(normal_nr++);
        else if(type == HEIGHT) location = "u_texture_height" + std::to_string(height_nr++);

        //std::cout << "Texture: " << "u_"+type+number << "\n";
        shader.set_uniform((location), (int)i);
        tm.bind_texture(textures[i], i);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    //std::cout << "\n\n\n";
}

void Mesh::setup_mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    /*Also for bones??*/

    glBindVertexArray(0);
}
