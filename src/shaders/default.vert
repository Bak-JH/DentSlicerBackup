#version 430 core

//from QAttribute names
in vec3 vertexPosition;
in vec4 vertexColor;

out EyeSpaceVertex {
    vec3 position;
    vec4 color;
} vs_out;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;

void main()
{

    //in world coordinates
    vs_out.position = vec3( modelMatrix * vec4( vertexPosition, 1.0 ) );
    vs_out.color = vertexColor;
    gl_Position = mvp * vec4( vertexPosition, 1.0 );

}
