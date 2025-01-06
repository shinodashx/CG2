#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"
#include <iostream>

class Control
{
public:
    Control();
    // build from another control
    Control(const Control& control);
    ~Control();
    
    void setSpeed(float speed);
    void setTarget(Vec3f target);
    void setPos(Vec3f pos);
    void setUp(Vec3f up);
    void setControllable(bool isControllable);
    void setPhi(float pho);
    void setTheta(float theta);
    void setRadius(float radius);
    void setLastX(float lastX);
    void setLastY(float lastY);

    float getSpeed();
    Vec3f getTarget();
    Vec3f getPos();
    Vec3f getUp();
    bool getControllable();
    float getPhi();
    float getTheta();
    float getRadius();
    float getLastX();
    float getLastY();

    void moveForward(float last_time);
    void moveBackward(float last_time);
    void moveLeft(float last_time);
    void moveRight(float last_time);
    void moveUp(float last_time);
    void moveDown(float last_time);
    void rotate(float x, float y);
    void zoom(float x, float y, float last_time);
    void update();
    void reset();

    void setTrackingMode(int trackingMode);
    int getTrackingMode();

    void updatePos(float last_time);

    Mat44f getViewMatrix();
	float kPi_ = 3.1415926f;

    void setForward(bool isForward);
    void setBackward(bool isBackward);
    void setLeft(bool isLeft);
    void setRight(bool isRight);
    void setUp(bool isUp);
    void setDown(bool isDown);



private:
    bool isControllable;
    float phi, theta;
    float radius;
    float speed;
    float mouseSensitivity;

    int trackingMode;

    float lastX, lastY;
    Vec3f target = {0.f, 0.f, 0.f};
    Vec3f pos = {0.f, 0.f, 0.f};
    Vec3f up = {0.f, 0.f, 0.f};

    bool isForward;
    bool isBackward;
    bool isLeft;
    bool isRight;
    bool isUp;
    bool isDown;
};