#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform float u_CullRadius;
uniform sampler2D texture1;
uniform vec3 emissionColor;
uniform float emissionIntensity;

const int MAX_LIGHTS = 127;
uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];

uniform float gamma;
uniform float ambientLight;

void main()
{
    if (length(FragPos) < u_CullRadius) discard;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    vec3 lighting = vec3(0.0);

    // Directional Light
    for (int i = 0; i < numLights; i++)
    {
        vec3 lightDir = normalize(-lightPositions[i]);

        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColors[i] * lightIntensities[i];

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColors[i] * lightIntensities[i];

        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColors[i] * lightIntensities[i];

        lighting += ambient + diffuse + specular;
    }

    vec3 texColor = texture(texture1, TexCoord).rgb;
    vec3 finalColor = (lighting + ambientLight) * texColor + emissionColor * emissionIntensity;
    
    FragColor = vec4(pow(finalColor, vec3(1.0 / gamma)), 1.0);
}
