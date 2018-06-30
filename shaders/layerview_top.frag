#version 330 core

in LayerViewVertex {
    vec3 position;
    vec3 worldPos;
    vec4 color;
    vec3 normal;
} fs_in;

out vec4 fragColor;

uniform float height;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

void calcLight( vec3 position, vec3 norm, out vec3 amb, out vec3 diff, out vec3 spec )
{
    vec3 LightColor = vec3(1, 1, 1);
    vec3 LightPosition = vec3(0, 0, 0);
    vec3 n = normalize( norm );
    vec3 s = normalize( LightPosition - position );
    vec3 v = normalize( -position );
    vec3 r = reflect( -s, n );

    float sDotN = max( dot( s, n ), 0.0 );
    amb = LightColor * ambient;
    diff = LightColor * diffuse * sDotN;
    spec = LightColor * specular * pow( max( dot(r,v) , 0.0 ), 0 );
}

void main()
{
    if( fs_in.worldPos.z < height ) {
        discard;//fragColor = vec4(1, 0, 0, 0.5);
    } else {
        vec3 amb, diff, spec;
        calcLight( fs_in.position, fs_in.normal, amb, diff, spec );

        fragColor = vec4( amb + diff, 1 ) * fs_in.color + vec4( spec, 1 );
        fragColor.a = fs_in.color.a;
    }

}
