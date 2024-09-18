#version 330 core

#include "Assets/ShaderLibrary/Lighting.glsl"

in vec2 uv;
in vec3 positionWS;
in vec3 normalWS;
in vec3 tangentWS;
in vec3 bitangentWS;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;

out vec4 FragColor;

void main()
{
    vec3 baseColor = texture(_MainTex, uv).rgb;
    mat3 TBN = mat3(tangentWS, bitangentWS, normalWS);
    vec3 normalTS = texture(_NormalTex, uv).rgb * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalTS);
    vec3 color = BilingPhongLight(normal, positionWS, baseColor);
    FragColor = vec4(color, 1.0);
}