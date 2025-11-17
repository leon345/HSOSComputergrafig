#pragma once

class Perlin
{
    static const int PERM_SIZE = 256;
    static int p[PERM_SIZE * 2];
    static const int permutation[PERM_SIZE];
    static bool initialized;
public:
    Perlin();
    ~Perlin();
    Perlin(unsigned int seed);
    static float noise(float x, float y, float z);
private:
    static float fade(float t);
    static float lerp(float t, float a, float b);
    static float grad(int hash, float x, float y, float z);
    static void initPermutation();
};
