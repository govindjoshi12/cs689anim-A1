#include "Animation.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <algorithm>
#include <random>

using namespace std;

Animation::Animation() {
	
}

Animation::Animation(string RESOURCE_DIR, int numKeyframes, float keyframeInterval, 
					float yOffset, long unsigned int seed) {
						
	float dist = max(minKeyframeInterval, abs(keyframeInterval));
	glm::vec3 tVec = glm::vec3(0, yOffset, 0);

	this->seed = seed;
	create3x3x3RandomKeyframes(dist, numKeyframes, tVec);
	helicopter = Helicopter(RESOURCE_DIR);
	tfc = TimeControlFunction();
}

Animation::~Animation()
{
    
}

void Animation::create3x3x3RandomKeyframes(float dist, int numKeyframes, glm::vec3 origin) {
	vector<glm::vec3> points;
	for(int i = -1; i <= 1; i++) {
		for(int j = 0; j <= 2; j++) {
			for(int k = -1; k <= 1; k++) {
				if(i == 0 && j == 0 && k == 0) {
					continue;
				}

				glm::vec3 p = glm::vec3(i * dist, j * dist, k * dist) + origin;
				points.push_back(p);
			}
		}
	}
	vector<glm::vec3> axes(points);

	// https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector
	auto rng = std::default_random_engine { seed };
	shuffle(begin(points), end(points), rng);
	shuffle(begin(axes), end(axes), rng);

	// Doubling up on first and last points
	keyframes.push_back(Keyframe(origin));
	keyframes.push_back(Keyframe(origin));

	srand(seed);
	numKeyframes = min(numKeyframes, (int)(points.size()));
	for(int i = 0; i < numKeyframes; i++) {
		// Generate a random angle between 0 and 360 that's a multiple of 10
		float angle = (float)((int)(((double) rand() / (RAND_MAX)) * 36) * 10);
		keyframes.push_back(Keyframe(points[i], angle, axes[i]));
	}

	keyframes.push_back(Keyframe(origin));
	keyframes.push_back(Keyframe(origin));
}

void Animation::setSpline(Spline s) {
	spline = s;
}

vector<Keyframe> Animation::getKeyframes() {
	return keyframes;
}

glm::mat3 Animation::getHelicopterMatrix() {
	return currHelicopterMatrix;
}

float Animation::calculateU(double t, ArcTraversal arc) {
	float u = 0.0f;
	float tmax = (float)(keyframes.size()) * 2;
	float smax = spline.getSmax();
	float tNorm = std::fmod(t, tmax) / tmax;

	switch(arc) {
		case NALP: {
			// Interpolation
			float uMax = (double)(keyframes.size()) - 3;
			u = fmod(t, uMax);
			break;
		}
		case ALP: {
			float tNorm = std::fmod(t, tmax) / tmax;
			float sNorm = tNorm;
			float s = smax * sNorm;
			u = spline.s2u(s);
			break;
		}
		case EASE_IN_OUT: {
			// s = -2t^3 + 3t^2
			float sNorm = -2.0f * pow(tNorm, 3) + 3.0f * pow(tNorm, 2);
			float s = smax * sNorm;
			u = spline.s2u(s);
			break;

		}
		case CUSTOM1: {
			float sNorm = pow(tNorm, 2);
			float s = smax * sNorm;
			u = spline.s2u(s);
			break;
		} case CUSTOM2: {
			// TODO
			float sNorm = tfc.computeCubic(tNorm);
			float s = smax * sNorm;
			u = spline.s2u(s);
			break;
		}
		default: {
			break;
		}
	}

	return u;
}

void Animation::render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> P,
					 shared_ptr<MatrixStack> MV, shared_ptr<Camera> camera, 
					 double t, bool showKeyframes, ArcTraversal arc, bool helicopterCamera, bool rotateHC) {

	float u = calculateU(t, arc);
	Keyframe kf = spline.interpolate(u);

	if(helicopterCamera) {
		glm::mat4 heliCam(1.0f);

		// Translate to helicopter, then rotate,
		// then offset camera behind helicopter in local coord system

		heliCam *= glm::translate(heliCam, kf.locvec());

		if(rotateHC) {
			heliCam *= kf.rotmat();
		}

		// Need to make sure to use identity matrix so that we are translating
		// with respect to the local coordinate system. Was initially using heliCam
		// as the matrix argument. That was wrong and not sure what that means geometrically.

		heliCam *= glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		heliCam *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));

		MV->multMatrix(glm::inverse(heliCam));
	}

	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
    glUniform3f(prog->getUniform("kd"), 1.0f, 0.0f, 0.0f);

	helicopter.draw(prog, MV, t, false, kf.locvec(), kf.rotmat());

	if(showKeyframes) {
		for(Keyframe kf : keyframes) {
			helicopter.draw(prog, MV, t, true, kf.locvec(), kf.rotmat());
		}
	}

	// Reset uniform
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
}
