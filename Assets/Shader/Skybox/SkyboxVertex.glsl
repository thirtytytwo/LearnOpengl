#version 330 core

layout (location = 0) in vec3 positionOS;

#include "Assets/ShaderLibrary/Common.glsl"

out vec3 texCoord;

void main()
{
    texCoord = positionOS;
    mat3 view = mat3(ViewMatrix);
    vec4 pos = ProjectionMatrix * mat4(view) * vec4(positionOS, 0.0);
    gl_Position = pos.xyww;
}