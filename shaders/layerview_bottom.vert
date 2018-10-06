#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;

out LayerViewVertex {
    vec3 position;
    vec3 worldPos;
    vec4 color;
    vec3 normal;
} vs_out;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    if( vertexColor.r == 1 ) {
        vs_out.color = vec4(0.259, 0.749, 0.8, 1);
    } else {
        vs_out.color = vec4(0.431, 0.929, 0.196, 1);
    }
    vs_out.position = vec3( modelView * vec4( vertexPosition, 1.0 ) );
    vs_out.worldPos = vertexPosition;
    vs_out.normal = normalize(modelViewNormal * vertexNormal);
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
