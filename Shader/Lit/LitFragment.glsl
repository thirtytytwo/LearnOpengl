#version 330 core

#include "ShaderLibrary/Lighting.glsl"

in vec2 uv;
in vec3 positionWS;
in vec3 normalWS;

uniform sampler2D _MainTex;

out vec4 FragColor;

void main()
{
    vec3 baseColor = texture(_MainTex, uv).rgb;
    vec3 color = BilingPhongLight(normalWS, positionWS, baseColor);
    FragColor = vec4(color, 1.0);
}