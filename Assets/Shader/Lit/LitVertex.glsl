#version 330 core

layout (location = 0) in vec3 positionOS;
layout (location = 1) in vec3 normalOS;
layout (location = 2) in vec3 tangentOS;
layout (location = 3) in vec3 bitangentOS;
layout (location = 4) in vec4 color;
layout (location = 5) in vec2 inputUV;

#include "Assets/ShaderLibrary/Common.glsl"

out Varying
{
    vec2 uv;
    vec3 positionWS;
    vec3 normalWS;
    vec3 tangentWS;
    vec3 bitangentWS;
}output;

void main()
{

    gl_Position = TransformObjectToHClip(vec4(positionOS, 1.0));
    output.positionWS = TransformObjectToWorld(positionOS);
    output.normalWS = TransformObjectToWorldNormal(normalOS);
    output.tangentWS = TransformObjectToWorldDir(tangentOS);
    output.bitangentWS = TransformObjectToWorldDir(bitangentOS);
    output.uv = inputUV;
}