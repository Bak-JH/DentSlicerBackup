#version 430 core

uniform vec3 singleColor;
#pragma include flat.geom
layout (std430, binding = 0) buffer input
{
    uint ddd[];
};
void main()
{
    // calculate flat normal
    vec3 norm = calcNorm();
    for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm, singleColor);
        VertexColor = color;
        EmitVertex();
    }
    EndPrimitive();
    
}
