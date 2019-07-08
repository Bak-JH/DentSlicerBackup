#version 330 core

#pragma include singleColorImpl.geom

smooth out vec3 ModelPosition;
uniform mat4 inverseModelMatrix;

void main()
{
	// calculate flat normal
    vec3 norm = calcNorm();
	for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm);
        VertexColor = color;
        ModelPosition =  vec3( inverseModelMatrix * vec4( gs_in[i].position, 1.0 ) );
		EmitVertex();
	}
    EndPrimitive();
}
