#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    sampler2D roughness;    
    float shininess;
}; 

struct Light {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
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
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Utility function for the geometry term
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

void main()
{
    vec3 albedo = texture(material.diffuse, TexCoords).rgb;
    //rougness
    float roughness = texture(material.roughness, TexCoords).r;
    vec3 N = normalize(vec3(0.0, 0.0, 1.0));

    // ambient
    vec3 ambient = light.ambient * albedo;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    
    vec3 L = lightDir;  // Light direction
    vec3 V = viewDir; // View direction
    vec3 H = normalize(V + L);               // Halfway vector
    vec3 F0 = vec3(0.04); // Default reflectivity
    F0 = mix(F0, albedo, 0.2); // Mix with albedo

    // Fresnel
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    // Geometry
    float G = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);

    // Distribution
    float D = DistributionGGX(N, H, roughness);

    // Cook-Torrance BRDF
    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // Prevent divide by zero
    specular = numerator / denominator;

    // Combine results
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - 0.5; // Assuming metallic value is 0.5

    float NdotL = max(dot(N, L), 0.0);
    vec3 irradiance = vec3(1.0f) * NdotL;

    diffuse = kD * albedo / PI;

    vec3 result = (diffuse + specular);
        
    // vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 