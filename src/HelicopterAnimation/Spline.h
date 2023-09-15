#include <glm/glm.hpp>
#include "Keyframe.h"

using namespace std;

class Spline
{
public:
    Spline();
    Spline(vector<Keyframe> kfs);
    virtual ~Spline();
    Keyframe interpolate(float u);
    void drawSpline(bool drawLine = false, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f));
    float getSmax();
    float s2u(float s);

private:
    vector<glm::vec3> controlPoints;
    vector<glm::quat> quats;
    glm::mat4 B;
    float deltaU;
    float uMax;
    void drawSPoints();
    void drawCPLine();
    vector<pair<float, float>> usTable;
    void buildTable();
    glm::vec3 computeSplinePoint(glm::mat4 G, glm::mat4 B, float u);
};