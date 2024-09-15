struct Light
{
    vec3 position;
    vec3 direction;
    vec3 color;
};

uniform Light mainLight;
uniform vec3 CameraPosition;

vec3 BilingPhongLight(vec3 normal, vec3 positionWS, vec3 baseColor )
{
    vec3 viewDir = normalize(CameraPosition - positionWS);
    vec3 color = baseColor * mainLight.color;
    vec3 H = normalize(mainLight.direction + viewDir);
    float NdotL = dot(normal, mainLight.direction) * 0.5 + 0.5;
    float NdotH = dot(normal, H);
    float spec = pow(max(NdotH, 0.0), 32.0);
    float diff = NdotL;
    vec3 result = (color * diff + color * spec);
    return result;
}