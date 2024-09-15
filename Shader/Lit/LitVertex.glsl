#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

#include "ShaderLibrary/Common.glsl"

out vec2 uv;
out vec3 positionWS;
out vec3 normalWS;

void main()
{

    gl_Position = TransformObjectToHClip(vec4(aPos, 1.0));
    positionWS = aPos;
    normalWS = aNormal;
    uv = aUV;
}