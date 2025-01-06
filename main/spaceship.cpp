#include "spaceship.hpp"
#include <iostream>
#include "rectangle.hpp"

SpaceShip::SpaceShip()
{
    numVertices = 0;
    isRunning = false;
    initPos = Vec3f{-21.6833f, -0.97f, -37.0551f};
    pos = Vec3f{-21.6833f, -0.97f, -37.0551f};
    speedx = 0.0f;
    speedy = 0.0f;
    speedz = 0.0f;
    accx = 0.0f;
    accy = 0.0f;
    accz = 0.0f;
}

Vec3f SpaceShip::getPos() { return pos; }

void SpaceShip::setPos(Vec3f pos) { this->pos = pos; }

void SpaceShip::moveUp(float duration)
{
    // TODO: acceleration

    // if (isRunning)
    // {
    //   std::cout << "speed: " << speedy << std::endl;
    //   pos.y += speedy + 0.5f * accy * duration * duration;
    //   speedy += accy * duration;
    // }
    if (isRunning)
    {
        // x acceleration y acceleration z acceleration
        int odd = rand() % 2;
        float rd = rand() % 100;
        rd = rd / 100;
        if (odd == 0)
        {
            rd = -rd;
        }

        // uncomment this to make the spaceship move randomly
        rd = 0;

        accx += 0.001f;

        pos.x += speedx + 0.5f * accx * duration * duration + rd;
        pos.y += speedy + 0.5f * accy * duration * duration;
        pos.z += speedz + 0.5f * accz * duration * duration + rd;
        speedx += accx * duration;
        speedy += accy * duration;
        speedz += accz * duration;
        // std::cout << "speedx: " << speedx << " "
        //           << "speedy: " << speedy << " "
        //           << "speedz: " << speedz << std::endl;

        // if (pos.y > 100)
        // {
        //     accx = -accx;
        //     accy = -accy;
        //     accz = -accz;
        //     speedx = -speedx;
        //     speedy = -speedy;
        //     speedz = -speedz;
        // }
        // if (pos.y < 10) {
        //   reset();
        // }
    }
}

float SpaceShip::getRotateXY()
{
    return atan(speedx / speedy);
}

void SpaceShip::reset()
{
    this->pos = this->initPos;
    speedx = 0.0f;
    speedy = 0.0f;
    speedz = 0.0f;
    accx = 0.0f;
    accy = 0.0f;
    accz = 0.0f;
    isRunning = false;
}

SpaceShip::~SpaceShip() {}

GLuint SpaceShip::createSpaceShip()
{
    auto spaceship_body =
        make_cylinder(true, 20, Vec3f{0.2f, 0.2f, 0.2f},
                      kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                          make_scaling(1.5f, 0.2f, 0.2f));
    // auto spaceship_body = make_cube(Vec3f{0.2f, 0.2f, 0.2f},
    //                                 kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
    //                                     make_scaling(1.5f, 0.2f, 0.2f) * make_translation({0.6f, 0.0f, 0.f}));
    numVertices += spaceship_body.positions.size();

    auto spaceship_header = make_cone(
        true, 180, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({1.5f, 0.f, 0.f}) * make_scaling(0.5f, 0.2f, 0.2f));
    numVertices += spaceship_header.positions.size();

    auto spaceship_win_1 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({0.f, -0.1f, -0.2f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_1.positions.size();

    auto spaceship_win_2 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({0.f, 0.1f, -0.2f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_2.positions.size();

    auto spaceship_win_3 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({0.f, -0.1f, 0.2f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_3.positions.size();

    auto spaceship_win_4 = make_cube(
        Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({0.f, 0.1f, 0.2f}) * make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_4.positions.size();

    auto spaceship_escape_1 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({1.5f, 0.f, 0.f}) * make_scaling(1.2f, 0.1f, 0.1f));
    numVertices += spaceship_escape_1.positions.size();

    // left
    auto spaceship_escape_2 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},

        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({0.f, 0.f, -2.f}) * make_scaling(0.2f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_3 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({0.f, 0.f, -2.2f}) * make_scaling(0.3f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_4 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({0.f, 0.f, -2.4f}) * make_scaling(0.2f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    // right
    auto spaceship_escape_5 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({-.2f, 0.f, -2.f}) * make_scaling(0.2f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_6 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({-.3f, 0.f, -2.2f}) * make_scaling(0.35f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_7 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_x(3.1415926f / 2.f) *
            make_translation({-.2f, 0.f, -2.4f}) * make_scaling(0.2f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();

    // left stright line
    auto spaceship_escape_8 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({2.35f, 0.2f, 0.f}) * make_scaling(0.3f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_9 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({1.75f, 0.2f, 0.f}) * make_scaling(0.3f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    // right stright line
    auto spaceship_escape_10 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({2.35f, -0.2f, 0.f}) * make_scaling(0.3f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_11 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({1.75f, -0.2f, 0.f}) * make_scaling(0.3f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();

    auto spaceship_escape_12 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({2.0f, 0.3f, 0.f}) * make_scaling(0.25f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();
    auto spaceship_escape_13 = make_cylinder(
        true, 20, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({2.0f, -0.3f, 0.f}) * make_scaling(0.25f, 0.035f, 0.035f));
    numVertices += spaceship_escape_2.positions.size();

    auto escapeship_head = make_cone(
        true, 180, Vec3f{0.2f, 0.2f, 0.2f},
        kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
            make_translation({2.4f, 0.f, 0.f}) * make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_header.positions.size();

    auto spaceship_window_1 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({1.2f, -0.1f, -0.11f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_1.positions.size();

    auto spaceship_window_2 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({1.2f, 0.1f, -0.11f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_2.positions.size();

    auto spaceship_window_3 =
        make_cube(Vec3f{0.2f, 0.2f, 0.2f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({1.2f, -0.1f, 0.11f}) *
                      make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_3.positions.size();

    auto spaceship_window_4 =
        make_cube(Vec3f{1.f, 1.f, 0.f},
                  kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                      make_translation({1.2f, 0.1f, 0.11f}) * make_scaling(0.5f, 0.1f, 0.1f));
    numVertices += spaceship_win_4.positions.size();

    auto spaceship = concatenate(spaceship_body, spaceship_header);
    spaceship = concatenate(spaceship, spaceship_win_1);
    spaceship = concatenate(spaceship, spaceship_win_2);
    spaceship = concatenate(spaceship, spaceship_win_3);
    spaceship = concatenate(spaceship, spaceship_win_4);
    spaceship = concatenate(spaceship, spaceship_escape_1);
    spaceship = concatenate(spaceship, spaceship_escape_2);
    spaceship = concatenate(spaceship, spaceship_escape_3);
    spaceship = concatenate(spaceship, spaceship_escape_4);
    spaceship = concatenate(spaceship, spaceship_escape_5);
    spaceship = concatenate(spaceship, spaceship_escape_6);
    spaceship = concatenate(spaceship, spaceship_escape_7);

    spaceship = concatenate(spaceship, spaceship_escape_8);
    spaceship = concatenate(spaceship, spaceship_escape_9);
    spaceship = concatenate(spaceship, spaceship_escape_10);
    spaceship = concatenate(spaceship, spaceship_escape_11);
    spaceship = concatenate(spaceship, spaceship_escape_12);
    spaceship = concatenate(spaceship, spaceship_escape_13);

    spaceship = concatenate(spaceship, escapeship_head);

    // spaceship = concatenate(spaceship, spaceship_window_1);
    // spaceship = concatenate(spaceship, spaceship_window_2);
    // spaceship = concatenate(spaceship, spaceship_window_3);
    // spaceship = concatenate(spaceship, spaceship_window_4);

    // std::cout << "spaceship" << std::endl;

    return create_vao(spaceship);
}

std::size_t SpaceShip::getNumVertices() { return numVertices; }

void SpaceShip::setRunning(bool status)
{
    isRunning = status;
    if (isRunning == true)
    {
        speedx = 0.0f;
        speedy = 0.0f;
        speedz = 0.0f;
        accx = 0.001f;
        accy = 0.1f;
        accz = 0.02f;
    }
}