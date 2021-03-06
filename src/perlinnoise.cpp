#include "PerlinNoise.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

PerlinNoise::PerlinNoise()
{
    p = {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
            43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
            97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
            107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

    p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::noise(double x, double y, double z)
{
    //Calculatethe unit cube the point lies in
    int xi = (int)x & 255;
    int yi = (int)y & 255;
    int zi = (int)z & 255;

    //Calculate the coordinates (relative to that cube) of the point
    double xf = x - (int)x;
    double yf = z - (int)y;
    double zf = y - (int)z;

    //Fade curves
    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    //We are using a cube, get the coordinates of the 8 corners as indices
    //for the hash table
    int A   = p[xi]       + yi;
    int AA = p[A]       + zi;
    int AB = p[A + 1] + zi;

    int B   = p[xi + 1] + yi;
    int BA = p[B]       + zi;
    int BB = p[B + 1] + zi;

    double x1 = lerp(grad(p[AA], xf,     yf, zf),
                            grad(p[BA], xf - 1, yf, zf),
                            u);

    double x2 = lerp(grad(p[AB], xf,    yf - 1, zf),
                            grad(p[BB], xf -1, yf - 1, z),
                            u);

    double y1 = lerp(x1, x2, v);

    x1 = lerp(  grad(p[AA + 1], xf,      yf, zf - 1),
                    grad(p[BA + 1], xf - 1, yf, zf - 1),
                    u);

    x2 = lerp(  grad(p[AB + 1], xf,      yf - 1, zf - 1),
                    grad(p[BB + 1], xf - 1, yf - 1, zf - 1),
                    u);

    double y2 = lerp(x1, x2, v);

    //Range between 0 and -1
    return (lerp(y1, y2, w) + 1) / 2.0f;

    //Values between -1 and 1
//    return ( lerp( y1, y2, w ) );

}

double PerlinNoise::octaveNoise(double x, double y, double z, int octaves, double persistence)
{
    double noiseValue = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;

    for(int i = 0; i < octaves; ++i)
    {
        noiseValue += noise(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;

        frequency *= 2;
    }

    return noiseValue/maxValue;
}

double PerlinNoise::fade(double t)
{
    //The fade function defined by Ken Perlin.

    //This essentially does 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::grad(int hash, double x, double y, double z)
{
    //This uses a different version to Ken Perlin's grad function which was proposed here:
    //
    //http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
    //

    switch(hash & 0xF)
    {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0; // never happens
    }
}

double PerlinNoise::lerp(double a, double b, double t)
{
    return a + t * (b - a);
}
