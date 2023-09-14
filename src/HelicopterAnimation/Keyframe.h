#include "../Shape.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;

class Program;

class Keyframe 
{
public:
    Keyframe(glm::vec3 point);
    virtual ~Keyframe();
    glm::vec3 locvec();
    glm::quat rotquat();

private:
    glm::vec3 loc;
    glm::quat rot;
};