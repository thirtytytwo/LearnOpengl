#pragma once

#include "Model.h"

class GameObject
{
public:
    GameObject(vec3 position = vec3(0), vec3 rotation = vec3(0), string modelPath = "");
    ~GameObject();

    void SetModel(string modelPath);
    void Translate(vec3 position);
    void Rotate(vec3 rotation);

    mat4 GetWorldMatrix();
private:
    vec3 position;
    vec3 rotation;
    Model model;
};
