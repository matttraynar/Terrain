#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

class PerlinNoise
{
public:
    // Initialize with the reference values for the permutation vector
    PerlinNoise();

    // Get a noise value, for 2D images z can have any value
    double noise(double x, double y, double z);
    double octaveNoise(double x, double y, double z, int octaves, double persistence);

private:
    double fade(double t);
    double grad(int hash, double x, double y, double z);
    double lerp(double a, double b, double t);

    // The permutation vector
    std::vector<int> p;
};

#endif
