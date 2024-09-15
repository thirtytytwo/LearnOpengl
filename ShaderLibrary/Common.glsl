#ifndef _COMMON_GLSL_
#define _COMMON_GLSL_

uniform mat4 _VPMatrix;

#define MATRIX_VP _VPMatrix;

vec4 TransformObjectToGClip(vec4 positionOS)
{
    return MATRIX_VP * positionOS;
}
#endif