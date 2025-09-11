#version 130
varying highp vec2 qt_TexCoord0;
uniform sampler2D source;
uniform sampler2D lut3d;

int lutSize = textureSize(lut3d, 0).x/2;

vec3 lutValue(ivec3 p)
{
    vec3 a0 = texelFetch(lut3d, ivec2(p.b, p.r*lutSize + p.g), 0).rgb; // low part
    vec3 a1 = texelFetch(lut3d, ivec2(p.b + lutSize, p.r*lutSize + p.g), 0).rgb; // high part
    return vec3(255.0)*(a0 + vec3(256.0)*a1)/vec3(256.0*256.0 - 1.0);
}

void main() {
    vec4 point = texture(source, qt_TexCoord0);
    ivec3 i0 = ivec3(int(floor(point.r*(lutSize - 1.0))),
                     int(floor(point.g*(lutSize - 1.0))),
                     int(floor(point.b*(lutSize - 1.0))) );

    if (i0.r == lutSize - 1) i0.r--;
    if (i0.g == lutSize - 1) i0.g--;
    if (i0.b == lutSize - 1) i0.b--;

    ivec3 i1 = i0 + ivec3(1);

    vec3 xyz0 = i0 / vec3(lutSize - 1.0);
    vec3 xyz1 = i1 / vec3(lutSize - 1.0);

    vec3 d = point.rgb - xyz0;
    vec3 d1 = xyz1 - xyz0;

    ivec3 iv;
    vec3 p000, p001, p010, p011, p100, p101, p110, p111;
    vec3 c1,c2,c3;

    iv[0] = i0[0];iv[1] = i0[1];iv[2] = i0[2];
    p000 = lutValue(iv);
    iv[0] = i0[0];iv[1] = i0[1];iv[2] = i1[2];
    p001 = lutValue(iv);
    iv[0] = i0[0];iv[1] = i1[1];iv[2] = i0[2];
    p010 = lutValue(iv);
    iv[0] = i0[0];iv[1] = i1[1];iv[2] = i1[2];
    p011 = lutValue(iv);
    iv[0] = i1[0];iv[1] = i0[1];iv[2] = i0[2];
    p100 = lutValue(iv);
    iv[0] = i1[0];iv[1] = i0[1];iv[2] = i1[2];
    p101 = lutValue(iv);
    iv[0] = i1[0];iv[1] = i1[1];iv[2] = i0[2];
    p110 = lutValue(iv);
    iv[0] = i1[0];iv[1] = i1[1];iv[2] = i1[2];
    p111 = lutValue(iv);

    if (d.x >= d.y && d.y >= d.z)
    {
        c1 = p100 - p000;
        c2 = p110 - p100;
        c3 = p111 - p110;
    }
    else
        if (d.x >= d.z && d.z >= d.y)
        {
            c1 = p100 - p000;
            c2 = p111 - p101;
            c3 = p101 - p100;
        }
        else
            if (d.z >= d.x && d.x >= d.y)
            {
                c1 = p101 - p001;
                c2 = p111 - p101;
                c3 = p001 - p000;
            }
            else
                if (d.y >= d.x && d.x >= d.z)
                {
                    c1 = p110 - p010;
                    c2 = p010 - p000;
                    c3 = p111 - p110;
                }
                else
                    if (d.y >= d.z && d.z >= d.x)
                    {
                        c1 = p111 - p011;
                        c2 = p010 - p000;
                        c3 = p011 - p010;
                    }
                    else // d.z >= d.y && d.y >= d.x
                    {
                        c1 = p111 - p011;
                        c2 = p011 - p001;
                        c3 = p001 - p000;
                    }

    vec3 c = p000 + c1*d.x/d1.x + c2*d.y/d1.y + c3*d.z/d1.z;
    //vec3 c = p000;

    gl_FragColor.rgb = c;
    gl_FragColor.a = point.a;
}

