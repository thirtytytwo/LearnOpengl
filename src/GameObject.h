#pragma once

#include "Model.h"

class GameObject
{
public:
    GameObject(vec3 position = vec3(0), vec3 rotation = vec3(0), vec3 scale = vec3(1), string modelPath = ""):position(position), rotation(rotation), scale(scale), model(modelPath){}
    
    void Translate(vec3 position);
    void Rotate(vec3 rotation);

    mat4 GetWorldMatrix();

    void Render(Camera &camera, Light &light);
    void FinishRender();
private:
    vec3 position;
    vec3 rotation;
    vec3 scale;
    Model model;
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

inline void GameObject::Render(Camera  &camera, Light &light)
{
    model.Render(camera, light, GetWorldMatrix());
}

inline void GameObject::FinishRender()
{

}