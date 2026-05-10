#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord1;
out vec2 TexCoord2;
out vec3 Normal;
out float Visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    float tide = sin(time * 0.5) * 0.05;
    vec4 worldPos = model * vec4(aPos.x, aPos.y + tide, aPos.z, 1.0);
    
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 posRelativeToCam = view * worldPos;
    gl_Position = projection * posRelativeToCam;
    
    TexCoord1 = aTexCoord + vec2(time * 0.02, time * 0.02);
    TexCoord2 = aTexCoord + vec2(-time * 0.01, -time * 0.03);
    
    float distance = length(posRelativeToCam.xyz);
    Visibility = exp(-pow((distance * 0.01), 2.0));
    Visibility = clamp(Visibility, 0.0, 1.0);
}