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
#include <vector>
#include "spaceship.hpp"
#include "particle.hpp"
#include "cylinder.hpp"
#include "rectangle.hpp"


ParticleSystem::ParticleSystem()
{
    isRunning = false;
    lastpos = Vec3f{0.f, -0.97f, 0.f};
}

ParticleSystem::~ParticleSystem(){}

Vec3f ParticleSystem::randomVec3f(const Vec3f& spaceshipPos, float max, float min){
    float x = spaceshipPos.x + min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
    float y = spaceshipPos.y + min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
    float z = spaceshipPos.z + min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
    return Vec3f{x, y, z};
}




void ParticleSystem::update(float duration, const Vec3f& spaceshipPos){
    int num = std::min(20, static_cast<int>(duration * 1000.0f));
    float x = spaceshipPos.x;
    float y = spaceshipPos.y;
    float z = spaceshipPos.z;
    if (x == 0 && y == 0.97 && z == 0)
    {
        return;
    }

    for (std::size_t i = 0; i < particles.size(); i++)
    {
        if (particles[i].second.time > 0.0f)
        {
            particles[i].second.time -= duration;
            particles[i].second.vel.y -= 0.1f * duration;
            particles[i].second.pos.x += particles[i].second.vel.x * duration;
            particles[i].second.pos.y += particles[i].second.vel.y * duration;
            particles[i].second.pos.z += particles[i].second.vel.z * duration;
            if (particles[i].second.pos.y < 0.0f)
            {
                particles[i].second.time = 0.0f;
            }
        } else {
            particles.erase(particles.begin() + i);
        }
    }
}

std::vector<std::pair<GLuint, particle>> ParticleSystem::createParticles(const Vec3f& spaceshippos, float duration){
    std::vector<std::pair<GLuint, particle>> now;
    if(isRunning == false){
        return now;
    }
    float x = spaceshippos.x;
    float y = spaceshippos.y;
    float z = spaceshippos.z;
    Vec3f pos = {x, y, z};
    int num = std::min(20, static_cast<int>(duration * 1000000.0f));
    for (int  i = num; i > 0; i--)
    {
        Vec3f pos = GeneratePoswithRange(lastpos, pos);
        float rdx = rand() % 100;
        float rdy = rand() % 100;
        float rdz = rand() % 100;
        rdx = rdx / 100;
        rdy = rdy / 100;
        rdz = rdz / 100;
        // 176, 2, 2
        //  Vec3f{0.90f, 0.02f, 0.2f}
        // auto body = make_cylinder(true, 20, Vec3f{176.0f/255.0f, 2.0f/255.0f, 2.0f/255.0f},
        //             kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
        //                 make_scaling(1.f, 1.f, 1.f));
        auto body = make_cube(Vec3f{176.0f/255.0f, 2.0f/255.0f, 2.0f/255.0f},
                    kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
                        make_scaling(0.1f, 0.1f, 0.1f));
        auto vao = create_vao(body);
        particle p;
        p.pos = pos;
        p.vel = Vec3f{rdx, rdy, rdz};
        p.time = 2.0f;
        p.num = i;
        std::pair<GLuint, particle> pair = std::make_pair(vao, p);
        now.push_back(pair);
    }
    lastpos = spaceshippos;
    return now;
}
Vec3f ParticleSystem::GeneratePoswithRange(Vec3f& lastpos, Vec3f& nowpos){
    float x = lastpos.x + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(nowpos.x-lastpos.x)))/4;
    float y = lastpos.y + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(nowpos.y-lastpos.y)))/4;
    float z = lastpos.z + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(nowpos.z-lastpos.z)))/4;
    return Vec3f{x, y, z}; 
}


void ParticleSystem::setRunning(bool status){
    isRunning = status;
}

void ParticleSystem::reset(){
    isRunning = false;
    lastpos = Vec3f{0.f, -0.97f, 0.f};
}

bool ParticleSystem::getRunning(){
    return isRunning;
}