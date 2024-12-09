#version 330 core

#include "Assets/ShaderLibrary/Common.glsl"
#include "Assets/ShaderLibrary/Lighting.glsl"

in vec2 uv;

uniform sampler2D GPosition;
uniform sampler2D GNormal;
uniform sampler2D GAlbedoSpec;

out vec4 FragColor;

void main()
{
    vec3 position = texture(GPosition, uv).rgb;
    vec3 normal = texture(GNormal, uv).rgb;
    vec3 albedo = texture(GAlbedoSpec, uv).rgb;

    vec3 color = BilingPhongLight(normal, position, albedo);
    FragColor = vec4(color, 1.0);
}