#version 430 core

uniform vec4 singleColor;
#pragma include flat.geom
void main()
{
    // calculate flat normal
    vec3 norm = calcNorm();
    for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec4 color = calcLights( gs_in[i].position, norm, singleColor);
        VertexColor = color;
        EmitVertex();
    }
    EndPrimitive();
}
