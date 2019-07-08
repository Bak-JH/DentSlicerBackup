#version 330 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in EyeSpaceVertex {
    vec3 position;
    vec3 normal;
} gs_in[];

//normal value can be either vertex normal or face normal depending on smoothing option
// out FinalVertex {
//     vec3 position;
//     vec3 normal;
//     vec3 color;
// } gs_out;
flat out vec3 VertexColor;
smooth out vec3 ModelPosition;

//light calculation
//uniform int pointLightCount; this is hard coded into 4 instead, due to static loop constraint on some devices
uniform vec3 ambient;
uniform vec3 diffuse;
uniform mat4 inverseModelMatrix;

//only use single color for all faces
uniform vec3 singleColor
#pragma include light.inc.frag

//run once per primitive
vec3 calcLights(in vec3 position, in vec3 norm)
{
    vec3 diffTotal = vec3(0,0,0);
    vec3 s;
    float dotSN;
    vec3 diff;
    for(int i = 0; i < 4; ++i)
    {
        s = normalize(  lights[i].position - position );
        //for now, light color is white, so don't bother multiplying by 1.0
        dotSN = max( dot( s, norm ), 0.0 );
        diff = diffuse * dotSN;
        diffTotal += diff;
    }

    return (ambient + diffTotal) * singleColor;
}

void main()
{
	// calculate flat normal
	vec3 oa = gs_in[1].position-gs_in[0].position;
	vec3 ob = gs_in[2].position-gs_in[0].position;
	vec3 norm=normalize(cross(oa, ob));
	for(int i=0; i<3; i++){
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm);
        VertexColor = color;
        ModelPosition =  vec3( inverseModelMatrix * vec4( gs_in[i].position, 1.0 ) );
		EmitVertex();
	}
    EndPrimitive();
}
