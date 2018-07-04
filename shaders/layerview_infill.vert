#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out LayerViewVertex {
    vec3 position;
    vec3 worldPos;
    vec4 color;
} vs_out;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    vs_out.color = vec4(1, 1, 0, 1);
    vs_out.position = vec3( modelView * vec4( vertexPosition, 1.0 ) );
    vs_out.worldPos = vertexPosition;
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
