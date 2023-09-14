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
    void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t);
    void toggleShowKeyframes();

private:
    vector<Keyframe> keyframes;
    Helicopter helicopter;
    Spline spline;
    bool showKeyframes;
};