﻿#version 330 core

#include "Assets/ShaderLibrary/Common.glsl"
#include "Assets/ShaderLibrary/Lighting.glsl"

in Varying
{
    vec2 uv;
    vec3 positionWS;
    vec3 normalWS;
    vec3 tangentWS;
    vec3 bitangentWS;
}input;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;

uniform samplerCube Skybox;

out vec4 FragColor;

void main()
{
    vec3 baseColor = texture(_MainTex, input.uv).rgb;
    mat3 TBN = mat3(input.tangentWS, input.bitangentWS, input.normalWS);
    vec3 normalTS = texture(_NormalTex, input.uv).rgb * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalTS);
    vec3 color = BilingPhongLight(normal, input.positionWS, baseColor);
    FragColor = vec4(color, 1.0);
}