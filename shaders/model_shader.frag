#version 330 core


out vec4 fragColor;

in FinalVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} fs_in;

void main()
{
    //test with this! if you see red, shader compiles without runtime error at least
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    fragColor = vec4(fs_in.color, 1.0);
}