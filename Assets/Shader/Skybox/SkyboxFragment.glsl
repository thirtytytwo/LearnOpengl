#version 330 core
out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube MainTex;

void main()
{    
    FragColor = texture(MainTex, texCoord);
}