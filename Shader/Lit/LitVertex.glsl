#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec2 UV;
out vec3 NORMAL;

uniform mat4 _VPMatrix;
void main()
{
    vec4 positionCS = _VPMatrix * vec4(aPos, 1.0);
    vec3 positionNDC = positionCS.xyz / positionCS.w;
    gl_Position = vec4(positionNDC, 1.0);
    NORMAL = aNormal;
    UV = aUV;
}