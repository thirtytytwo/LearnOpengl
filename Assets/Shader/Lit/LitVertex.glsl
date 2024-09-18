#version 330 core

layout (location = 0) in vec3 positionOS;
layout (location = 1) in vec3 normalOS;
layout (location = 2) in vec3 tangentOS;
layout (location = 3) in vec3 bitangentOS;
layout (location = 4) in vec4 color;
layout (location = 5) in vec2 inputUV;

#include "Assets/ShaderLibrary/Common.glsl"

out vec2 uv;
out vec3 positionWS;
out vec3 normalWS;
out vec3 tangentWS;
out vec3 bitangentWS;

void main()
{

    gl_Position = TransformObjectToHClip(vec4(positionOS, 1.0));
    positionWS = TransformObjectToWorld(positionOS);
    normalWS = TransformObjectToWorldNormal(normalOS);
    tangentWS = TransformObjectToWorldDir(tangentOS);
    bitangentWS = TransformObjectToWorldDir(bitangentOS);
    uv = inputUV;
}