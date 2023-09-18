#include <glm/glm.hpp>
#include "Helicopter.h"
#include "Spline.h"
#include "../Program.h"
#include "../Camera.h"
#include "TimeControl.h"

using namespace std;

class Keyframe;
class Camera;

enum ArcTraversal {
	NALP = 0,
	ALP,
	EASE_IN_OUT,
	CUSTOM1,
    CUSTOM2,
    AT_COUNT
};

class Animation
{
public:
    Animation();
    Animation(string RESOURCE_DIR, int numKeyframes, float keyframeInterval, float yOffset, long unsigned int seed);
    virtual ~Animation();
    void create3x3x3RandomKeyframes(float dist, int numKeyframes, 
                                    glm::vec3 translateVec = glm::vec3(0, 0, 0));
    void setSpline(Spline s);
    void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> P,
                shared_ptr<MatrixStack> MV, shared_ptr<Camera> camera, 
                double t, bool showKeyframes, ArcTraversal arc, bool helicopterCamera, bool rotateHC);
    vector<Keyframe> getKeyframes();
    glm::mat3 getHelicopterMatrix();

private:
    float yOffset;
    long unsigned int seed;

    float minKeyframeInterval = 0.1f;
    float maxKeyframeInterval = 10.0f;

    vector<Keyframe> keyframes;
    Helicopter helicopter;
    Spline spline;
    glm::mat3 currHelicopterMatrix;
    TimeControlFunction tfc;

    float calculateU(double t, ArcTraversal arc);
};