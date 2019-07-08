#version 330 core

#pragma include singleColorImpl.geom

void main()
{
	// calculate flat normal
    vec3 norm = calcNorm();
	for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm);
        VertexColor = color;
		EmitVertex();
	}
    EndPrimitive();
}
