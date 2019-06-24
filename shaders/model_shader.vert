#version 330 core

//from QAttribute names
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;

out EyeSpaceVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} vs_out;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;

void main()
{
    //in world coordinates
    vs_out.normal = normalize( modelNormalMatrix * vertexNormal );
    vs_out.position = vec3( modelMatrix * vec4( vertexPosition, 1.0 ) );
    vs_out.color = vertexColor;

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
