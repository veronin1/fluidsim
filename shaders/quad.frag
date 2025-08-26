#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    float value = texture(texture1, TexCoord).r;
    FragColor = vec4(value, value, value, 1.0);
}
