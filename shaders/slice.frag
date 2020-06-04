#version 330 core
out vec4 FragColor;
uniform float maxBright;

void main()
{
    FragColor = vec4(maxBright);
}
