#pragma once
#include <corecrt_math_defines.h>

#include "Model.h"

class Model;

class Coin : public Model
{
public:
    Coin(const char* file, Vector pos) : Model(file, false), state(Idle), time(0.0f), basePos(pos)
    {
        setIsCollectable(true);
        setIsCollidable(false);
        initialPosition = pos;
    }

    void update(float dtime);
    void collect();
private:
    enum State { Idle, Collected } state;
    float time;
    float collectTime = 0.0f;
    Vector basePos;
    Vector initialPosition;
};
