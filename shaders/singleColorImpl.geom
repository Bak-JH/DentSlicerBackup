
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in EyeSpaceVertex {
    vec3 position;
    vec3 normal;
} gs_in[];

flat out vec3 VertexColor;
uniform vec3 ambient;
uniform vec3 diffuse;

//only use single color for all faces
uniform vec3 singleColor;
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
vec3 calcNorm()
{
	vec3 oa = gs_in[1].position-gs_in[0].position;
	vec3 ob = gs_in[2].position-gs_in[0].position;
	return normalize(cross(oa, ob));
}

