#include "Mathf.h"

float Mathf::clamp(float value, float min, float max)
{
    return (value < min) ? min : (value > max) ? max : value;
}


