#pragma once

#include "MeshRenderer.h"

class GameObject
{
public:
    GameObject(vec3 position = vec3(0), vec3 rotation = vec3(0), vec3 scale = vec3(1)):position(position), rotation(rotation), scale(scale){}
    
    void Translate(vec3 position);
    void Rotate(vec3 rotation);
    void SetupMeshRenderer(std::string mesh);
    
    mat4 GetWorldMatrix();
    
    void Setup();
private:
    vec3 position;
    vec3 rotation;
    vec3 scale;
    MeshRenderer* renderer{nullptr};
};

inline void GameObject::Translate(vec3 position)
{
    this->position += position;
}

inline void GameObject::Rotate(vec3 rotation)
{
    this->rotation += rotation;
}

inline mat4 GameObject::GetWorldMatrix()
{
    mat4 modelMatrix = mat4(1.0f);
    modelMatrix = translate(modelMatrix, position);
    modelMatrix = rotate(modelMatrix, glm::radians(rotation.x), vec3(1, 0, 0));
    modelMatrix = rotate(modelMatrix, glm::radians(rotation.y), vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, glm::radians(rotation.z), vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

inline void GameObject::SetupMeshRenderer(std::string mesh)
{
    renderer = new MeshRenderer(mesh);
}

inline void GameObject::Setup()
{
    renderer->Setup(GetWorldMatrix());
}