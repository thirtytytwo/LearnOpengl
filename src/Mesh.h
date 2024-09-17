#pragma once

#include <glm.hpp>
#include <stdexcept>
#include <utility>
#include <vector>
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

struct Vertex
{
    vec4 color;
    vec3 position;
    vec3 tangent; //only uv[0]
    vec3 normal;
    vec2 uv[3];
};
class Mesh
{
public:
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
    ~Mesh();

    unsigned int GetIndicesNum();
    unsigned int VAO;
private:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int VBO, EBO;
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

inline Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
    // normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // uv0
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[0]));
    // uv1
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[1]));
    // uv2
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv[2]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline unsigned int Mesh::GetIndicesNum()
{
    return static_cast<unsigned int> (indices.size());
}


