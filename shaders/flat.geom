
//legokangpalla minimalistic flat shading:
// 1. don't do specular calculation, as flat shading causes specular to highlight entire face
// 2. do the light calculation for all light sources once and re-use the result.
// 3. Use only point lights and assume color is 1 with intensity of 1(more complext light is useless for simple flat shading in CAD)
// 4. Light count is hard coded so that the compiler can unroll it(HOPEFULLY!)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in EyeSpaceVertex {
    vec3 position;
    vec4 color;
} gs_in[];

//since flat shader, all fragments are colored with same color
flat out vec4 VertexColor;
uniform vec3 ambient;
uniform vec3 diffuse;

//only use single color for all faces
#pragma include light.inc.frag

//run once per primitive
vec4 calcLights(in vec3 position, in vec3 norm, in vec4 primitiveColor)
{
    vec3 diffTotal = vec3(0,0,0);
    vec3 s;
    float dotSN;
    vec3 diff;
    for(int i = 0; i < 4; ++i)
    {
        // s = normalize(  lights[i].position - position );
        s = normalize(lights[i].direction);
        //for now, light color is white, so don't bother multiplying by 1.0
        dotSN = max( dot( s, norm ), 0.0 );
        diff = diffuse * dotSN;
        diffTotal += diff;
    }

    return vec4((ambient + diffTotal), 1.0) * primitiveColor;
}
vec3 calcNorm()
{
	vec3 oa = gs_in[1].position-gs_in[0].position;
	vec3 ob = gs_in[2].position-gs_in[0].position;
	return normalize(cross(oa, ob));
}

