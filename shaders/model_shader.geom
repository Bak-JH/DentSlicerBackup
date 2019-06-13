#version 330 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in EyeSpaceVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} gs_in[];

//normal value can be either vertex normal or face normal depending on smoothing option
out FinalVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} gs_out;

//light calculation
//uniform int pointLightCount; this is hard coded into 4 instead, due to static loop constraint on some devices
uniform vec3 pointLightPositions[];
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform mat4 modelView;
//we do light calculation in per primitive fashion
vec3 calcLight(in vec3 pLightPos, in vec3 position, in vec3 norm, in vec3 pVertColor)
{
    // //for now, light color is white, so don't bother multiplying by 1.0
    vec3 eyeLightPos =  vec3( modelView * vec4( pLightPos, 1.0 ) );
    vec3 s = normalize( eyeLightPos - position );
    vec3 v = normalize( -position );
    vec3 r = reflect( -s, norm );

    float dotSN = max( dot( s, norm ), 0.0 );
    // // vec3 amb = pLightColor * ambient;
    // // vec3 diff = pLightColor * diffuse * sDotN;
    // // vec3 spec = pLightColor * specular * pow( max( dot(r,v) , 0.0 ), 0 );
    vec3 diff = diffuse * dotSN;
    vec3 spec = specular * pow( max( dot(r,v) , 0.0 ), 0 );
    return ( ambient + diff ) * pVertColor +  spec;
}

void main()
{
	// calculate flat normal
	vec3 oa = gs_in[1].position-gs_in[0].position;
	vec3 ob = gs_in[2].position-gs_in[0].position;
	vec3 norm=normalize(cross(oa, ob));
	for(int i=0; i<3; i++){
        //return vtx normal or face normal
        gs_out.normal = norm;
        gs_out.position = gs_in[i].position;
        gl_Position = gl_in[i].gl_Position;
        //do light calculation for each point light
        vec3 color = vec3(0, 0, 0);
        //don't for loop this! unrolled due to error FFS
        color += calcLight(pointLightPositions[0], gs_in[i].position, norm, gs_in[i].color);
        color += calcLight(pointLightPositions[1], gs_in[i].position, norm, gs_in[i].color);
        color += calcLight(pointLightPositions[2], gs_in[i].position, norm, gs_in[i].color);
        color += calcLight(pointLightPositions[3], gs_in[i].position, norm, gs_in[i].color);
        gs_out.color = color;
		EmitVertex();
	}
    EndPrimitive();
}
