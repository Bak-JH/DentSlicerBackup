#version 430 core
out vec4 fragColor;
uniform vec4 singleColor;


// in EyeSpaceVertex {
//     vec3 position;
//     vec3 normal;
// } notUsed;

void main()
{
    fragColor = singleColor;
    //fragColor = vec4(0.0, 0.0, 0.0 , 1.0);
}
