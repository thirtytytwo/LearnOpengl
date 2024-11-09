#pragma once

#include <glm.hpp>
#include <stdexcept>
#include <utility>
#include <vector>
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "RenderUtils.h"

using namespace glm;
using namespace std;

struct Vertex
{
    vec4 color;
    vec3 position;
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;
    vec2 uv[3];
};
class Mesh
{
public:
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

    void Setup(Buffer &buffer);
private:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int VBO, EBO, VAO;
    void SetupMesh();
};

inline Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
{
    if (vertices.empty() || indices.empty())
    {
        throw std::invalid_argument("ERROR: vertex or index data is empty");
    }
    this->vertices = std::move(vertices);
    this->indices  = std::move(indices);
    SetupMesh();
}

inline void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // tangent
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // bitangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    // color
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
    // uv0
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[0]));
    // uv1
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[1]));
    // uv2
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[2]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void Mesh::Setup(Buffer &buffer)
{
    // glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
    // glActiveTexture(GL_TEXTURE0);
    buffer.VAO = VAO;
    buffer.indices = static_cast<unsigned int>(indices.size());
}

