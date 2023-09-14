#include <glm/glm.hpp>
#include "Keyframe.h"

using namespace std;

class Spline
{
public:
    Spline();
    Spline(vector<glm::vec3> cps);
    virtual ~Spline();
    glm::vec3 splineFunc(float u);
    void drawSpline(bool drawLine = false, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f));

private:
    vector<glm::vec3> controlPoints;
    glm::mat4 B;
    float deltaU;
    float uMax;
    void drawCPLine();
};