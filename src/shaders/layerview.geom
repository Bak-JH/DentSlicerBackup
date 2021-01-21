#version 430 core
#pragma include flat.geom
//per primitive color code
//color look up table
//for layer effect
smooth out vec3 ModelPosition;
uniform mat4 inverseModelMatrix;
uniform mat4 fuckingStuipidWorldMatrix;
uniform vec4 singleColor;


void main()
{
    vec3 norm = calcNorm();
	for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec4 color = calcLights( gs_in[i].position, norm,  singleColor);
        VertexColor = color;
        ModelPosition =  vec3( fuckingStuipidWorldMatrix* (inverseModelMatrix * vec4( gs_in[i].position, 1.0 )) );
		EmitVertex();
	}
    EndPrimitive();
}
