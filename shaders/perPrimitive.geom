#version 430 core
#pragma include flat.geom



void main()
{

    vec3 norm = calcNorm();


    for(int i=0; i<3; i++){
        //return vtx normal or face normal
        gl_Position = gl_in[i].gl_Position;
        vec4 color = calcLights( gs_in[i].position, norm, gs_in[0].color);
        VertexColor = color;
        EmitVertex();
    }
    EndPrimitive();
    

}
