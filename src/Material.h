#pragma once

#include <filesystem>
#include <string>
#include <utility>

#include "RenderUtils.h"
#include "Shader.h"
#include "Texture.h"

using namespace std;

class Material
{
public:
    Material(vector<Texture> textures, string shaderName = "");
    Material(Texture texture, string shaderName = ""):Material(vector<Texture>{texture}, std::move(shaderName)){}

    void SetupTexture(int index, string location);
    void Setup(Buffer &buffer);
private:
    vector<Texture> textures;
    Shader shader;
    
};

inline Material::Material(vector<Texture> textures, string shaderName):shader(Shader(std::move(shaderName)))
{
    this->textures = std::move(textures);
    shader.use();
    //TODO:后续加入别的贴图类别
    shader.SetInt("_MainTex", 0);
    shader.SetInt("_NormalTex", 1);
}

inline void Material::SetupTexture(int index, string location)
{
    shader.use();
    shader.SetInt(location, index);
}


inline void Material::Setup(Buffer &buffer)
{
    //图片是要每帧都传的，有时间再弄清楚这几个关键字之前跟图片的区别
    //如果不是每帧传，在GL_TEXTURE2D这个target中可能会出现有些图片每帧传然后把其他的图片覆盖掉，都改成了最新的绑定图片地址
    // shader.use();
    // shader.SetWolrd(worldMatrix);
    // shader.SetCameraProps(camera);
    // shader.SetLight(light);
    // this->textures[0].Active(GL_TEXTURE0);
    // this->textures[0].Bind();
    // this->textures[1].Active(GL_TEXTURE1);
    // this->textures[1].Bind();
    buffer.shader = &shader;
    for (int i = 0; i < textures.size(); i++)
    {
        buffer.textures.push_back(&textures[i]);
    }
}


