#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse; // albedo
    sampler2D specular;  
    sampler2D normal;  
    sampler2D roughness; 
    float shininess;
}; 

struct Light {
    vec3 position;
    // vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos; // maybe same as worldPos
in vec3 Normal;  
in vec2 TexCoords;
in mat3 TBN;
  
uniform vec3 camPos;

uniform Material material;
uniform Light light;

// Constants
const float PI = 3.14159265359;

// Utility function to calculate the Fresnel effect
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Utility function for the distribution term (GGX)
float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001);
}

// Utility function for the geometry term
float GeometrySchlickGGX(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

    return ggx1 * ggx2;
}

void main()
{
    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    
    float roughness = texture(material.roughness, TexCoords).g;
    float metallic = texture(material.roughness, TexCoords).b;
    vec3 normalMap = texture(material.normal, TexCoords).rgb;


    vec3 N = 2.0 * texture(material.normal, TexCoords).rgb - 1.0;
	N = normalize(TBN * N);

    vec3 L0 = vec3(0.0f);

    // vec3 N = normalize(normalMap * Normal);
    vec3 V = normalize(camPos - FragPos);
    vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic); // also known as F0 | last param is metallic

    vec3 L = normalize(light.position - FragPos); // light direction
    vec3 H = normalize(V + L);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.diffuse * attenuation * 30000.0; // diffuse = color * attenuation == "brightness"

    float NdotV = max(dot(N, V), 0.0000001);
    float NdotL = max(dot(N, L), 0.0000001);
    float HdotV = max(dot(H, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    // Distribution
    float D = DistributionGGX(NdotH, roughness);
    // Geometry
    float G = GeometrySchlickGGX(NdotV, NdotL, roughness);
    // Fresnel
    vec3 F = fresnelSchlick(HdotV, baseReflectivity);

    vec3 specular = D * G * F;
    specular /= 4.0 * NdotV * NdotL;

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic; // Assuming metallic value is 0.5

    L0 += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.04) * albedo;
    vec3 color = ambient + L0;

    // HDR
    // color = color / (color + vec3(1.0));

    // gamma correct
    // color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
} 