#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    float d = texture(texture1, TexCoord).r;
    d = clamp(d * 5.0, 0.0, 1.0); // amplify small changes
    FragColor = vec4(d, d, d, 1.0);
}

