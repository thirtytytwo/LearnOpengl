#pragma once
#include <utility>

#include "Shader.h"
#include "Texture.h"

using namespace std;
class Material
{
public:
    Material(aiMaterial * material = nullptr, string directory = "", string shaderName = "");
    ~Material();

    void LoadTextures(aiMaterial * material, aiTextureType type, string typeName, string directory);
    void SetShader(string name);
private:
    vector<Texture> textures;
    Shader shader;
    string directory;
};

inline Material::Material(aiMaterial * material, string directory, string shaderName)
{
    this->directory = directory;
    this->shader = Shader(std::move(shaderName));
    if (material)
    {
        LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        LoadTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        LoadTextures(material, aiTextureType_NORMALS, "texture_normal", directory);
        LoadTextures(material, aiTextureType_HEIGHT, "texture_height", directory);
    }
}

//TODO:完成这个函数
inline void Material::LoadTextures(aiMaterial * material, aiTextureType type, string typeName, string directory)
{
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        string path = directory + "/" + str.C_Str();
        textures.push_back(Texture(path.c_str(), typeName));
    }
}
