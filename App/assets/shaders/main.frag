#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Visibility;

uniform sampler2D texture1;
uniform vec3 lightDir;
uniform float lightIntensity;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    vec3 norm = normalize(Normal);
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diff * vec3(1.0) * lightIntensity;
    
    vec3 result = (ambient + diffuse) * texColor.rgb;
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    
    FragColor = vec4(mix(fogColor, result, Visibility), texColor.a);
}