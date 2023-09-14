#include "Animation.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

Animation::Animation() {
	
}

Animation::Animation(string RESOURCE_DIR) {
	float dist = 5.0f;

	// Doubling up on first and last points
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, 0.0f)));

	keyframes.push_back(Keyframe(glm::vec3(dist, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, dist, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, dist, dist)));
	keyframes.push_back(Keyframe(glm::vec3(-1.0f * dist, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, dist)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, -1.0f * dist)));
	keyframes.push_back(Keyframe(glm::vec3(dist, dist, -1.0f * dist)));

	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, 0.0f)));
	keyframes.push_back(Keyframe(glm::vec3(0.0f, 0.0f, 0.0f)));

	spline = Spline(keyframes);
	helicopter = Helicopter(RESOURCE_DIR);

	showKeyframes = true;
}

Animation::~Animation()
{
    
}

void Animation::toggleShowKeyframes() {
    showKeyframes = !showKeyframes;
}

void Animation::render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, double t) {

	if(showKeyframes) {
		for(Keyframe kf : keyframes) {
			helicopter.draw(prog, MV, t, kf.locvec(), true);
		}

		// Draw Spline
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		spline.drawSpline(true, glm::vec3(0.0f, 0.5f, 0.5f));
	}
}
