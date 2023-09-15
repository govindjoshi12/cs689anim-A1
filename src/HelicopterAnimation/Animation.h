#include <glm/glm.hpp>
#include "Helicopter.h"
#include "Spline.h"
#include "../Program.h"
#include "../Camera.h"

using namespace std;

class Keyframe;
class Camera;

class Animation
{
public:
    Animation();
    Animation(string RESOURCE_DIR, int numKeyframes);
    virtual ~Animation();
    void create3x3x3RandomKeyframes(float dist, int numKeyframes);
    void setSpline(Spline s);
    void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> P,
                shared_ptr<MatrixStack> MV, shared_ptr<Camera> camera, 
                double t, bool showKeyframes, bool alp, bool helicopterCamera);
    vector<Keyframe> getKeyframes();
    glm::mat3 getHelicopterMatrix();

private:
    vector<Keyframe> keyframes;
    Helicopter helicopter;
    Spline spline;
    glm::mat3 currHelicopterMatrix;
};