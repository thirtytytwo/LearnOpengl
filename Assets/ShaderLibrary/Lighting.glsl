#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED
struct Light
{
    vec4 position;
    vec4 color;    
    vec4 direction;
};

layout(std140) uniform LightBuffer
{
    Light mainLight;
};

vec3 BilingPhongLight(vec3 normal, vec3 positionWS, vec3 baseColor )
{
    vec3 viewDir = normalize(CameraPosition.xyz - positionWS);
    vec3 color = baseColor * mainLight.color.xyz;
    vec3 H = normalize(-mainLight.direction.xyz + viewDir);
    float NdotL = dot(normal, -mainLight.direction.xyz) * 0.5 + 0.5;
    float NdotH = dot(normal, H);
    float spec = pow(max(NdotH, 0.0), 32.0);
    float diff = NdotL;
    vec3 result = (color * diff + spec);
    return result;
}
#endif