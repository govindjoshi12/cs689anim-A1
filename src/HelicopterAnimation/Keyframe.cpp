#include "Keyframe.h"

Keyframe::Keyframe(glm::vec3 point) {
    loc = point;
    rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

Keyframe::Keyframe(glm::vec3 point, glm::quat q) {
    loc = point;
    rot = q;
}

Keyframe::Keyframe(glm::vec3 point, float angle, glm::vec3 axis)
{
    loc = point;
    axis = glm::normalize(axis);
    rot = glm::angleAxis(angle, axis);
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

glm::mat3 Keyframe::rotmat()
{
    return glm::mat4_cast(glm::normalize(rot));
}