#include <glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"
#include "simple_mesh.hpp"
#include "cylinder.hpp"
#include "cone.hpp"
#include <iostream>
#include "loadobj.hpp"

class SpaceShip
{
public:
    SpaceShip();
    ~SpaceShip();
    GLuint createSpaceShip();
    std::size_t getNumVertices();
    void setRunning(bool status);
    Vec3f getPos();
    void setPos(Vec3f pos);
    void moveUp(float duration);
    void reset();
    float getRotateXY();
    bool isRunning;

private:
    std::size_t numVertices;
    Vec3f pos;
    float speedx;
    float speedy;
    float speedz;
    float accx;
    float accy;
    float accz;
    Vec3f initPos;
};
