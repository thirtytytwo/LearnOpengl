#pragma once

#include <filesystem>
#include <string>
#include "Shader.h"
#include "Texture.h"

using namespace std;

class Material
{
public:
    Material(string shaderName = "");
    
private:
    vector<Texture> textures;
    Shader  shader;
};

inline Material::Material(vector<Texture> textures, string shaderName)
{
    this->shader = Shader(std::move(shaderName));
    this->textures = std::move(textures);

    //TODO:后续加入别的贴图类别
    this->textures[0].Active(GL_TEXTURE0);
    this->textures[1].Active(GL_TEXTURE1);

    shader.SetInt("_MainTex", 0);
    shader.SetInt("_NormalTex", 1);
}


