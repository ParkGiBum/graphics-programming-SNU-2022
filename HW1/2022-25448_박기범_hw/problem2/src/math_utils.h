#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <iostream>
#include <vector>
struct AABB{
    float left;
    float right;
    float top;
    float bottom;
};
float getRandomValueBetween(float a, float b)
{
    float r = (float) rand() / (float) RAND_MAX;
    return a * (1-r) + b * r;
}
#endif