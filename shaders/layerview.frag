#version 430 core

uniform float height;
out vec4 fragColor;
flat in vec3 VertexColor;
smooth in vec3 ModelPosition;

void main()
{
    if(ModelPosition.z < height)
    {
        fragColor = vec4(VertexColor, 1.0);
    }
    else
    {
        discard;
    }
}
