#include "HelicopterAnim.h"

HelicopterAnim::HelicopterAnim(string RESOURCE_DIR)
{
    helicopter = Helicopter(RESOURCE_DIR);

	float dist = 2.5f;
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(dist, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, dist, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, dist)));
}

HelicopterAnim::~HelicopterAnim()
{
    
}

void toggleShowKeyframes() {
    showKeyframes = !showKeyframes;
}

void render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t) {

}
