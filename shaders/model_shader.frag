#version 330 core


out vec4 fragColor;

in FinalVertex {
    vec3 position;
    vec3 normal;
    vec3 color;
} fs_in;

#pragma include light.inc.frag
    
void main()
{
    //test with this! if you see red, shader compiles without runtime error at least
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec3 redColr = vec3(1.0,0,0);
    bool isRed = false;
    for(int i = 0; i < lightCount ; ++i)
    {
        if(lights[i].color == redColr)
        {
            isRed = true;
        }
    }
    // if(isRed)
    // {
    //     fragColor = vec4(redColr, 1.0);
    // }
    // else
    // {
    //     fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }
    if(lightCount == 0)
    {
        fragColor = vec4(redColr, 1.0);
    }
    else
    {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }



    //fragColor = vec4(fs_in.color, 1.0);
}