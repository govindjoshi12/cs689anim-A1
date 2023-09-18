#include <glm/glm.hpp>
#include "Keyframe.h"

using namespace std;

class Spline
{
public:
    Spline();
    Spline(vector<Keyframe> keyframes, float tableDeltaU, float arcIntervalLength, bool GQ);
    virtual ~Spline();
    Keyframe interpolate(float u);
    void drawSpline(bool drawLine = false, glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f));
    float getSmax();
    float s2u(float s);

private:
    float tableDeltaU;
    float arcIntervalLength;

    vector<glm::vec3> controlPoints;
    vector<glm::quat> quats;
    glm::mat4 B;
    float deltaU;
    float uMax;
    void drawSPoints();
    void drawCPLine();
    vector<pair<float, float>> usTable;
    void buildTable(bool GQ);
    glm::vec3 computeSplinePoint(glm::mat4 G, glm::mat4 B, float u);
    float computeGQ(glm::mat4 G, glm::mat4 B, float u0, float u1);

    // GQ Vars
    float val = pow(3.0f / 5.0f, 0.5f);
    vector<float> xVals{-1 * val, 0, val};
    vector<float> weights{5.0f / 9.0f, 8.0f / 9.0f, 5.0f / 9.0f};
};