#version 430 core

uniform float height;
out vec4 fragColor;
flat in vec4 VertexColor;
smooth in vec3 ModelPosition;

void main()
{
    if(ModelPosition.z < height)
    {
        fragColor = VertexColor;
    }
    else
    {
        discard;
    }
}
