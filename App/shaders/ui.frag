#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D uiTexture;

void main()
{
    vec4 texColor = texture(uiTexture, TexCoords);
    if(texColor.a < 0.1) discard; 
    FragColor = texColor;
}