#ifndef _COMMON_GLSL_
#define _COMMON_GLSL_

uniform mat4 WorldMatrix;
uniform mat4 InvTransWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


//空间变换
vec3 TransformObjectToWorld(vec3 positionOS)
{
    vec4 result = WorldMatrix * vec4(positionOS, 1.0);
    return result.xyz;
}
vec4 TransformObjectToHClip(vec4 positionOS)
{
    vec4 result = ProjectionMatrix * ViewMatrix * WorldMatrix * positionOS;
    return result;
}
vec3 TransformObjectToWorldNormal(vec3 normalOS)
{
    vec4 result = InvTransWorldMatrix * vec4(normalOS, 0.0);
    normalize(result);
    return result.xyz;
}
vec3 TransformObjectToWorldDir(vec3 dirOS)
{
    vec4 result = WorldMatrix * vec4(dirOS, 0.0);
    return result.xyz;
}
#endif