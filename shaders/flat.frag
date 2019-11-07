#version 430 core
out vec4 fragColor;
flat in vec4 VertexColor;


// in EyeSpaceVertex {
//     vec3 position;
//     vec3 normal;
// } notUsed;

void main()
{
    //fragColor = vec4(0.5, 0.0, 0.0 , 1.0);
    fragColor = VertexColor;
}
