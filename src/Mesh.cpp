#include "Mesh.hpp"
#include <glad/glad.h>
#include <iostream>


//TODO: finish implementing(copying from learnopengl.com)
Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& ind, const std::vector<Texture>& text)
:   vertices(vert),
    indices(ind),
    textures(text)
{
    setup_mesh();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::Draw(ShaderProgram& shader)
{
    unsigned int diffuse_nr = 0;
    unsigned int specular_nr = 0;
    unsigned int normal_nr = 0;
    unsigned int height_nr = 0;

    shader.bind();
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string type = textures[i].type;

        if(type == "texture_diffuse") number = std::to_string(diffuse_nr++);
        else if(type == "texture_specular") number = std::to_string(specular_nr++);
        else if(type == "texture_normal") number = std::to_string(normal_nr++);
        else if(type == "texture_height") number = std::to_string(height_nr++);

        //std::cout << "Texture: " << "u_"+type+number << "\n";
        shader.set_uniform(("u_"+type+number), (int)i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
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
