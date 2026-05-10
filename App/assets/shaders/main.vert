#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out float Visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 posRelativeToCam = view * worldPos;
    gl_Position = projection * posRelativeToCam;
    
    TexCoord = aTexCoord;
    
    float distance = length(posRelativeToCam.xyz);
    Visibility = exp(-pow((distance * 0.10), 2.0));
    Visibility = clamp(Visibility, 0.0, 1.0);
}