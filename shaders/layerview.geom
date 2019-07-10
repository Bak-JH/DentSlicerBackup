#version 430 core
#pragma include flat.geom
//per primitive color code
uniform unsigned int perPrimitiveColorCode[];
//color look up table
uniform vec3 colorTable[];
//for layer effect
smooth out vec3 ModelPosition;
uniform mat4 inverseModelMatrix;

void main()
{
    vec3 norm = calcNorm();
    unsigned int currentColorCode = perPrimitiveColorCode[gl_PrimitiveID];
    vec3 currColor = colorTable[currentColorCode];
	for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm, currColor);
        VertexColor = color;
        ModelPosition =  vec3( inverseModelMatrix * vec4( gs_in[i].position, 1.0 ) );
		EmitVertex();
	}
    EndPrimitive();
}
