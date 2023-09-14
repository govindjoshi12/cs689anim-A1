#include "Animation.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

Animation::Animation() {
	
}

Animation::Animation(string RESOURCE_DIR) {
	float dist = 5.0f;

	// Doubling up on first and last points
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));


	// Todo 
	// Red: x, blue: z, green: y
	keyframes.push_back(Keyframe(glm::vec3(dist, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, dist, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, dist, dist)));

	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, 0, dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, dist)));

	keyframes.push_back(Keyframe(glm::vec3(dist, 0, dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, -1 * dist)));
	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, dist, -1 * dist)));

	keyframes.push_back(Keyframe(glm::vec3(dist, 0, -1 * dist)));
	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, dist, dist)));
	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, 0, -1 * dist)));

	keyframes.push_back(Keyframe(glm::vec3(0, 0, -1 * dist)));
	keyframes.push_back(Keyframe(glm::vec3(0, dist, -1 * dist)));
	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, dist, 0)));

	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, 0, dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, 0)));
	keyframes.push_back(Keyframe(glm::vec3(-1 * dist, 0, 0)));

	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));

	helicopter = Helicopter(RESOURCE_DIR);
}

Animation::~Animation()
{
    
}

void Animation::setSpline(Spline s) {
	spline = s;
}

vector<glm::vec3> Animation::getKeyframePositions() {

	vector<glm::vec3> positions;
	for(Keyframe k: keyframes) {
		positions.push_back(k.locvec());
	}
	return positions;

}

void Animation::render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t, bool showKeyframes) {

	if(showKeyframes) {
		for(Keyframe kf : keyframes) {
			helicopter.draw(prog, MV, t, kf.locvec(), true);
		}
		// Reset uniform
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	}

	// Animation
	float uMax = (double)(keyframes.size());
	float u = fmod(t, uMax);
	glm::vec3 translatedPos = spline.splineFunc(u);
	helicopter.draw(prog, MV, t, translatedPos, false);
}
