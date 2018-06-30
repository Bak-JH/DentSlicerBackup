#version 330 core

uniform float height;

in LayerViewVertex {
    vec3 position;
    vec3 worldPos;
    vec4 color;
} fs_in;

out vec4 fragColor;

void main()
{
    if( fs_in.worldPos.z > height ) {
        discard;//fragColor = vec4(1, 0, 0, 0.5);
    } else {
        fragColor = fs_in.color;
    }

}
