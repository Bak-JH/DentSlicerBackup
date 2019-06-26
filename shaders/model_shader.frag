#version 330 core


out vec4 fragColor;

// in FinalVertex {
//     vec3 position;
//     vec3 normal;
//     vec3 color;
// } fs_in;

flat in vec3 VertexColor;

void flatShade()
{
    fragColor = vec4(VertexColor, 1.0);
}
#ifndef MAIN
#define MAIN
void main()
{
    // fragColor = vec4(fs_in.color, 1.0);
    flatShade();
}
#endif