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

Animation::Animation(string RESOURCE_DIR, int numKeyframes) {
	float dist = 5.0f;
	create3x3x3RandomKeyframes(dist, numKeyframes);
	helicopter = Helicopter(RESOURCE_DIR);
}

Animation::~Animation()
{
    
}

void Animation::create3x3x3RandomKeyframes(float dist, int numKeyframes) {
	vector<glm::vec3> points;
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			for(int k = -1; k <=1; k++) {
				if(i == 0 && j == 0 && k == 0) {
					continue;
				}
				points.push_back(glm::vec3(i * dist, j * dist, k * dist));
			}
		}
	}
	vector<glm::vec3> axes(points);

	// https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector
	auto rd = std::random_device {}; 
	auto rng = std::default_random_engine { rd() };
	shuffle(begin(points), end(points), rng);
	shuffle(begin(axes), end(axes), rng);

	// Doubling up on first and last points
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));

	numKeyframes = min(numKeyframes, (int)(points.size()));
	for(int i = 0; i < numKeyframes; i++) {
		// Generate a random angle between 0 and 360 that's a multiple of 10
		float angle = (float)((int)(((double) rand() / (RAND_MAX)) * 36) * 10);
		keyframes.push_back(Keyframe(points[i], angle, axes[i]));
	}

	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));
	keyframes.push_back(Keyframe(glm::vec3(0, 0, 0)));
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

void Animation::render(const shared_ptr<Program> prog, shared_ptr<MatrixStack> P,
					 shared_ptr<MatrixStack> MV, shared_ptr<Camera> camera, 
					 double t, bool showKeyframes, bool alp, bool helicopterCamera) {

	float u;

	if(alp) {
		float tmax = (float)(keyframes.size());
		float smax = spline.getSmax();
		float tNorm = std::fmod(t, tmax) / tmax;
		float sNorm = tNorm;
		float s = smax * sNorm;
		u = spline.s2u(s);
	} else {
		// Interpolation
		float uMax = (double)(keyframes.size()) - 3;
		u = fmod(t, uMax);
	}

	Keyframe kf = spline.interpolate(u);

	P->pushMatrix();
	P->loadIdentity();

	MV->pushMatrix();
	MV->loadIdentity();

	if(helicopterCamera) {
		glm::mat4 heliCam(1.0f);

		heliCam *= glm::translate(heliCam, kf.locvec());
		heliCam *= glm::translate(heliCam, glm::vec3(3, 0, 0));
		
		heliCam *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		heliCam *= kf.rotmat();

		MV->multMatrix(glm::inverse(heliCam));
	} else {
		camera->applyViewMatrix(MV);
	}

	camera->applyProjectionMatrix(P);

	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
    glUniform3f(prog->getUniform("kd"), 1.0f, 0.0f, 0.0f);

	helicopter.draw(prog, MV, t, false, kf.locvec(), kf.rotmat());

	if(showKeyframes) {
		for(Keyframe kf : keyframes) {
			helicopter.draw(prog, MV, t, true, kf.locvec(), kf.rotmat());
		}
		// Reset uniform
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	}
}
