#version 430 core

uniform float minHeight;
uniform float maxHeight;
out vec4 fragColor;
flat in vec4 VertexColor;
smooth in vec3 ModelPosition;

void main()
{
    if(ModelPosition.z < maxHeight && ModelPosition.z > minHeight)
    {
        fragColor = VertexColor;
    }
    else
    {
        discard;
    }
}
