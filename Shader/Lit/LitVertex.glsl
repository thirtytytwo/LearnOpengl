#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

#include "ShaderLibrary/Common.glsl"

out vec2 UV;
out vec3 NORMAL;

uniform mat4 _VPMatrix;
void main()
{
    gl_Position = TransformObjectToGClip(vec4(aPos, 1.0));
    NORMAL = aNormal;
    UV = aUV;
}