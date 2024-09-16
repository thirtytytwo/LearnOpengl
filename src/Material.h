#pragma once
#include "Shader.h"
#include "Texture.h"

using namespace std;
class Material
{
public:
    
private:
    Texture mainTexture;
    Texture normalTexture;
    Shader shader;
};
