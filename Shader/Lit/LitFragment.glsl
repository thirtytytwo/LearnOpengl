#version 330 core

#include "ShaderLibrary/Common.glsl"

in vec2 UV;
in vec3 NORMAL;

uniform vec3 _LightPosition;
uniform vec3 _LightDirection;
uniform vec3 _LightColor;

uniform sampler2D _MainTex;

out vec4 FragColor;

void main()
{
    float NdotL = dot(NORMAL, _LightDirection) * 0.5 + 0.5;
    vec3 diffuse = _LightColor * NdotL;
    vec3 baseColor = texture(_MainTex, UV).rgb;
    FragColor = vec4(baseColor * diffuse, 1.0);
}