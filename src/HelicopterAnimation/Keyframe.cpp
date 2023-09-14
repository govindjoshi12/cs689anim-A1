#include "Keyframe.h"

Keyframe::Keyframe(glm::vec3 point)
{
    loc = point;
}

Keyframe::~Keyframe()
{

}

glm::vec3 Keyframe::locvec() 
{
    return loc;
}

glm::quat Keyframe::rotquat()
{
    return rot;
}