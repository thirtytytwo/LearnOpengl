#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include "Shader.h"
#include "Texture.h"

using namespace std;

class Material
{
public:
    Material(vector<Texture> textures, string shaderName = "");

    void Render(Camera  &camera, Light &light, mat4 worldMatrix);
private:
    vector<Texture> textures;
    Shader shader;
};

inline Material::Material(vector<Texture> textures, string shaderName):shader(Shader(std::move(shaderName)))
{
    this->textures = std::move(textures);

    //TODO:后续加入别的贴图类别
    this->textures[0].Active(GL_TEXTURE0);
    shader.SetInt("_MainTex", 0);
    this->textures[0].Bind();
    this->textures[1].Active(GL_TEXTURE1);
    shader.SetInt("_NormalTex", 1);
    this->textures[1].Bind();
}

inline void Material::Render(Camera  &camera, Light &light, mat4 worldMatrix)
{
    shader.use();
    shader.SetWolrd(worldMatrix);
    shader.SetCameraProps(camera);
    shader.SetLight(light);
}


