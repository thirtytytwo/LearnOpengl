#version 330 core

in vec2 UV;

uniform sampler2D _MainTex;
out vec4 FragColor;

void main()
{
    FragColor = texture(_MainTex, UV);
}