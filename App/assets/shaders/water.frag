#version 330 core
out vec4 FragColor;

in vec2 TexCoord1;
in vec2 TexCoord2;
in vec3 Normal;
in float Visibility;

uniform sampler2D texture1;
uniform vec3 lightDir;
uniform float lightIntensity;

void main()
{
    vec4 texColor1 = texture(texture1, TexCoord1);
    vec4 texColor2 = texture(texture1, TexCoord2);
    vec4 finalTexColor = mix(texColor1, texColor2, 0.5);
    
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    vec3 norm = normalize(Normal);
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diff * vec3(1.0) * lightIntensity;
    
    vec3 waterTint = vec3(0.2, 0.5, 0.8);
    vec3 result = (ambient + diffuse) * finalTexColor.rgb * waterTint;
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    
    FragColor = vec4(mix(fogColor, result, Visibility), 0.85);
}