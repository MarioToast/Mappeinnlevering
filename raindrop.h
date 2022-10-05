#ifndef RAINDROP_H
#define RAINDROP_H

#include "octahedronball.h"

class RainDrop : public OctahedronBall
{
public:
    RainDrop(int n);
    void init(GLint matrixUniform) override;
    void draw() override;
    int LifeTimer = 200;
    bool DoneInit = false;
    void DestroyDrop();
};

#endif // RAINDROP_H
