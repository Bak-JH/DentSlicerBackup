#version 330 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in EyeSpaceVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} gs_in[];

//normal value can be either vertex normal or face normal depending on smoothing option
// out FinalVertex {
//     vec3 position;
//     vec3 normal;
//     vec3 color;
// } gs_out;
flat out vec3 VertexColor;

//light calculation
//uniform int pointLightCount; this is hard coded into 4 instead, due to static loop constraint on some devices
uniform vec3 ambient;
uniform vec3 diffuse;
uniform mat4 modelView;

//legokangpalla minimalistic flat shading:
// 1. don't do specular calculation, as flat shading causes specular to highlight entire face
// 2. do the light calculation for all light sources once and re-use the result.
// 3. Use only point lights and assume color is 1 with intensity of 1(more complext light is useless for simple flat shading in CAD)
// 4. Light count is hard coded so that the compiler can unroll it(HOPEFULLY!)
#pragma include light.inc.frag

//run once per primitive
vec3 calcLights(in vec3 position, in vec3 norm, in vec3 pVertColor)
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

    return (ambient + diffTotal) * pVertColor;
}

void main()
{
	// calculate flat normal
	vec3 oa = gs_in[1].position-gs_in[0].position;
	vec3 ob = gs_in[2].position-gs_in[0].position;
	vec3 norm=normalize(cross(oa, ob));
    vec3 darkestColor = vec3(255, 255, 255);
    vec3 currColor;
    for(int i=0; i<3; i++){
        currColor = gs_in[i].color;
        darkestColor = (length(currColor) < length(darkestColor))? currColor : darkestColor; 
    }
	for(int i=0; i<3; i++){
        //return vtx normal or face normal
        // gs_out.normal = norm;
        // gs_out.position = gs_in[i].position;
        gl_Position = gl_in[i].gl_Position;
        vec3 color = calcLights( gs_in[i].position, norm, darkestColor);
        VertexColor = color;
        //gs_out.color = color;
		EmitVertex();
	}
    EndPrimitive();
}
