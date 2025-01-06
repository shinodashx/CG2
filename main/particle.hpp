#include <vector>
#include <glad.h>
#include <GLFW/glfw3.h>
#include "../vmlib/vec3.hpp"

struct particle{
    std::size_t num;
    Vec3f pos;
    Vec3f vel;
    bool isRunning;
    float time;
};


class ParticleSystem{
public:
    ParticleSystem();
    ~ParticleSystem();
    void update(float duration, const Vec3f& spaceshipPos);
    void emit(const Vec3f& pos);
    Vec3f randomVec3f(const Vec3f& spaceshipPos, float max, float min);
    std::vector<std::pair<GLuint, particle>> createParticles(const Vec3f& spaceshippos, float duration);
    Vec3f GeneratePoswithRange(Vec3f& lastpos, Vec3f& nowpos);
    void setRunning(bool status);
    void reset();
    bool getRunning();
    
private:
    std::vector<std::pair<GLuint, particle>> particles;
    bool isRunning;
    Vec3f lastpos;
    float nowpos;
};
