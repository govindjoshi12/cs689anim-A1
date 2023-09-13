#include <glm/glm.hpp>
#include "Helicopter.h"

using namespace std;

class HelicopterAnim
{
public:
    HelicopterAnim(string RESOURCE_DIR);
    virtual ~HelicopterAnim();
    void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t)
    void toggleShowKeyframes();

private:
    vector<Keyframe> keyframes;
    Helicopter helicopter;
    bool showKeyframes;
};