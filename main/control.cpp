#include "control.hpp"

Control::Control()
{
    isControllable = false;
    phi = 0.f;
    theta = 0.f;
    radius = 10.f;
    lastX = 0.f;
    lastY = 0.f;
    target = {0.f, 0.f, 1.f};
    pos = {1.f, 0.f, 0.f};
    up = {0.f, 1.f, 0.f};
    speed = 20.f;
    mouseSensitivity = 0.01f;

    isForward = false;
    isBackward = false;
    isLeft = false;
    isRight = false;
    isUp = false;
    isDown = false;

    trackingMode = 0;
}

Control::Control(const Control& control)
{
    isControllable = control.isControllable;
    phi = control.phi;
    theta = control.theta;
    radius = control.radius;
    lastX = control.lastX;
    lastY = control.lastY;
    target = control.target;
    pos = control.pos;
    up = control.up;
    speed = control.speed;
    mouseSensitivity = control.mouseSensitivity;

    trackingMode = control.trackingMode;
}

Control::~Control()
{
}

bool Control::getControllable()
{
    return isControllable;
}

float Control::getPhi()
{
    return phi;
}

float Control::getTheta()
{
    return theta;
}

float Control::getRadius()
{
    return radius;
}

float Control::getLastX()
{
    return lastX;
}

float Control::getLastY()
{
    return lastY;
}

float Control::getSpeed()
{
    return speed;
}

Vec3f Control::getTarget()
{
    return target;
}

Vec3f Control::getPos()
{
    return pos;
}

Vec3f Control::getUp()
{
    return up;
}

void Control::setControllable(bool isControllable)
{
    this->isControllable = isControllable;
}

void Control::setPhi(float pho)
{
    this->phi = pho;
}

void Control::setTheta(float theta)
{
    this->theta = theta;
}

void Control::setRadius(float radius)
{
    this->radius = radius;
}

void Control::setLastX(float lastX)
{
    this->lastX = lastX;
}

void Control::setLastY(float lastY)
{
    this->lastY = lastY;
}

void Control::setSpeed(float speed)
{
    this->speed = speed;
}

void Control::setTarget(Vec3f target)
{
    this->target = target;
}

void Control::setPos(Vec3f pos)
{
    this->pos = pos;
}

void Control::setUp(Vec3f up)
{
    this->up = up;
}

void Control::moveForward(float last_time)
{
    if (trackingMode != 0)
        return;
    Vec3f dir = normalize(target);
    dir.y = 0;
    pos -= dir * speed * last_time;
}

void Control::moveBackward(float last_time)
{
    if (trackingMode != 0)
        return;
    Vec3f dir = normalize(target);
    dir.y = 0;
    pos += dir * speed * last_time;
}

void Control::moveLeft(float last_time)
{
    if (trackingMode != 0)
        return;
    Vec3f left = normalize(cross_product(up, target));
    pos -= (left * speed) * last_time;
}

void Control::moveRight(float last_time)
{
    if (trackingMode != 0)
        return;
    Vec3f left = normalize(cross_product(up, target));
    pos += (left * speed) * last_time;
}

void Control::moveUp(float last_time)
{
    if (trackingMode != 0)
        return;
    pos.y += speed * last_time;
}

void Control::moveDown(float last_time)
{
    if (trackingMode != 0)
        return;
    pos.y -= speed * last_time;
}

void Control::updatePos(float last_time)
{
    if (isForward)
        moveForward(last_time);
    if (isBackward)
        moveBackward(last_time);
    if (isLeft)
        moveLeft(last_time);
    if (isRight)
        moveRight(last_time);
    if (isUp)
        moveUp(last_time);
    if (isDown)
        moveDown(last_time);
}

void Control::setForward(bool isForward)
{
    this->isForward = isForward;
}

void Control::setBackward(bool isBackward)
{
    this->isBackward = isBackward;
}

void Control::setLeft(bool isLeft)
{
    this->isLeft = isLeft;
}

void Control::setRight(bool isRight)
{
    this->isRight = isRight;
}

void Control::setUp(bool isUp)
{
    this->isUp = isUp;
}

void Control::setDown(bool isDown)
{
    this->isDown = isDown;
}



void Control::setTrackingMode(int trackingMode)
{
    this->trackingMode = trackingMode;
    if (trackingMode == 2) {
        // pos = {0.f, 1.f, -1.f};
        // target = {0.f, 0.f, 1.f};
        // up = {0.f, 1.f, 0.f};
    }
}

int Control::getTrackingMode()
{
    return trackingMode;
}

Mat44f Control::getViewMatrix()
{
    return make_rotation_x(theta) * make_rotation_y(phi) * make_translation({ 0.f, 0.f, radius }) * make_translation(-pos);
}

Vec3f rotateVector(Vec3f v, float deltaX, float deltaY)
{

    Mat44f rx = make_rotation_x(deltaY);
    Mat44f ry = make_rotation_y(deltaX);

    Vec4f v4 = {v.x, v.y, v.z, 0};
    v4 = rx * ry * v4;

    return Vec3f{v4.x, v4.y, v4.z};
}

void Control::rotate(float x, float y)
{
    if (trackingMode != 0)
        return;
    auto const deltaX = float(x - lastX);
    auto const deltaY = float(y - lastY);
    phi += deltaX * mouseSensitivity;
    theta += deltaY * mouseSensitivity;

    if (theta > kPi_ / 2.f)
        theta = kPi_ / 2.f;
    if (theta < -kPi_ / 2.f)
        theta = -kPi_ / 2.f;

    target = normalize(Vec3f{
        -(std::sin(phi)) * std::cos(theta),
        0,
        std::cos(phi) * std::cos(theta)});

    Vec3f center = pos + target;
    // pos = center - (rotateVector(target, deltaX, deltaY) * radius);
    lastX = x;
    lastY = y;
}

