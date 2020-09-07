#version 430 core
out vec4 fragColor;
uniform vec4 lineColor;


// in EyeSpaceVertex {
//     vec3 position;
//     vec3 normal;
// } notUsed;

void main()
{
    fragColor = lineColor;
    //fragColor = vec4(0.0, 0.0, 0.0 , 1.0);
}
