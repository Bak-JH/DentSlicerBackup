#version 330 core

uniform float height;
out vec4 fragColor;
flat in vec3 VertexColor;
smooth in vec3 ModelPosition;

void flatShade()
{
    fragColor = vec4(VertexColor, 1.0);
}
#ifndef MAIN
#define MAIN
void main()
{
    if(ModelPosition.z < height)
    {
        flatShade();
    }
    else
    {
        discard;
    }
}
#endif