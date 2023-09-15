#include "../Shape.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;

class Program;

class Keyframe 
{
public:
    Keyframe(glm::vec3 point);
    Keyframe(glm::vec3 point, glm::quat q);
    Keyframe(glm::vec3 point, float angle, glm::vec3 axis);
    virtual ~Keyframe();
    glm::vec3 locvec();
    glm::quat rotquat();
    glm::mat3 rotmat();

private:
    glm::vec3 loc;
    glm::quat rot;
};