#version 430 core
#pragma include flat.geom
//per primitive color code
// layout (std430, binding = 0) buffer perPrimitiveColorCode
// {
//     uint colorCodes[];
// } pcc_in;
// uniform uint perPrimitiveColorCode[200];
//color look up table
uniform vec3 colorTable[5];
layout (std430, binding = 1) buffer input
{
    uint ddd[];
};

void main()
{

    // calculate flat normal
    vec3 norm = calcNorm();
    // uint currentColorCode = perPrimitiveColorCode[gl_PrimitiveID];
    // uint currentColorCode = perPrimitiveColorCode[1];

    // uint currentColorCode =  pcc_in.colorCodes[0];
    // int currentColorCode = 0;


    uint currentColorCode = ddd[0];

    //vec3 currColor = vec3(0.5, 0.5, 0.5);
    vec3 currColor = colorTable[currentColorCode];
    for(int i=0; i<3; i++){
        //return vtx normal or face normal
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm, currColor);
        VertexColor = color;
        EmitVertex();
    }
    EndPrimitive();
    

}
