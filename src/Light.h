#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;

enum LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

class Light
{
public:
    LightType type;
    vec3 position;
    vec3 direction;
    vec3 color;

    Light(LightType type = DIRECTIONAL, vec3 position = vec3(100), vec3 direction = vec3(-1), vec3 color = vec3(1));
    ~Light();

    void Transform(float x, float y , float z);
    void Rotate(float x, float y, float z);
};

inline Light::Light(LightType type, vec3 position, vec3 direction, vec3 color)
{
    this->type = type;
    this->position = position;
    this->direction = direction;
    this->color = color;
}
inline Light::~Light()
{
    
}

