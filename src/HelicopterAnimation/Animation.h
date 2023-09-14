#include <glm/glm.hpp>
#include "Helicopter.h"
#include "Spline.h"
#include "../Program.h"

using namespace std;

class Keyframe;

class Animation
{
public:
    Animation();
    Animation(string RESOURCE_DIR);
    virtual ~Animation();
    void setSpline(Spline s);
    void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t, bool showKeyframes);
    vector<glm::vec3> getKeyframePositions();

private:
    vector<Keyframe> keyframes;
    Helicopter helicopter;
    Spline spline;
};