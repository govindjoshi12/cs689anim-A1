#include "Spline.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/quaternion.hpp>

using namespace std;

Spline::Spline()
{
}

Spline::Spline(vector<Keyframe> keyframes)
{
    int numKfs = (int)(keyframes.size());
    for(Keyframe k: keyframes) {
        controlPoints.push_back(k.locvec());
        quats.push_back(k.rotquat());
    }

    // Take short route for quat interpolation
    for(int i = 0; i < numKfs - 2; i++) {
        float dot = glm::dot(quats[0], quats[1]);
        if(dot < 0.0f) {
            quats[1] = -1.0f * quats[1];
        }
    }

	glm::mat4 catmullRom;
	catmullRom[0] = glm::vec4(0, 2, 0, 0);
	catmullRom[1] = glm::vec4(-1, 0, 1, 0);
	catmullRom[2] = glm::vec4(2, -5, 4, -1);
	catmullRom[3] = glm::vec4(-1, 3, -3, 1);
    B = 0.5f * catmullRom;

    deltaU = 0.01;
    uMax = (int)(controlPoints.size()) - 3;

    buildTable();
}

Spline::~Spline()
{
}

void Spline::buildTable()
{
	// INSERT CODE HERE
	usTable.clear();

	int ncps = (int)controlPoints.size();
	if(ncps < 4)
		return;

	usTable.push_back(make_pair(0.0f, 0.0f));

	float deltaU = 0.2;
	float currDist = 0;
	for(float u1 = deltaU; u1 <= ncps - 3; u1 += deltaU) {

		float u0 = u1 - deltaU;

		glm::mat4 Gk;
		float fcp = floor(u0);
		for(int i = fcp; i < fcp + 4; i++) {
			Gk[i - fcp] = glm::vec4(controlPoints[i], 0.0f);
		}

		glm::vec3 pA = computeSplinePoint(Gk, B, u0 - fcp); // u0 - fcp necessary for "concatenated u"
		glm::vec3 pB = computeSplinePoint(Gk, B, u1 - fcp);
		float deltaS = glm::length(pB - pA);
		currDist = currDist + deltaS;

		usTable.push_back(make_pair(u1, currDist));
	}
}

float Spline::s2u(float s)
{
	int idx = 0;
	float s1 = usTable[0].second;
	while(s1 < s) {
		idx++;
		s1 = usTable[idx].second;
	}

	float s0 = usTable[max(0, idx - 1)].second;

	float denom = s1 - s0;
	float alpha;
	if(denom == 0.0f) {
		alpha = 0;
	} else {
		alpha = (s - s0) / denom;
	} 

	float u0 = usTable[max(0, idx - 1)].first;
	float u1 = usTable[idx].first;

	float u = (1.0f - alpha) * u0 + alpha * u1;

	return u;
}

float Spline::getSmax() {
    float smax = usTable.back().second; // spline length
    return smax;
}

void Spline::drawCPLine()
{
    glBegin(GL_LINE_STRIP);
    for(glm::vec3 i : controlPoints) {
        glVertex3f(i.x, i.y, i.z);
    }
    glEnd();
}

void Spline::drawSPoints() {
    float ds = 1.0f;
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    float smax = getSmax();
    for(float s = 0.0f; s < smax; s += ds) {
        // Convert from s to (concatenated) u
        float uu = s2u(s);
        // Convert from concatenated u to the usual u between 0 and 1.
        float kfloat;
        float u = std::modf(uu, &kfloat);
        // k is the index of the starting control point
        int k = (int)std::floor(kfloat);
        // Compute spline point at u
        glm::mat4 Gk;
        for(int i = 0; i < 4; ++i) {
            Gk[i] = glm::vec4(controlPoints[k+i], 0.0f);
        }
        glm::vec4 uVec(1.0f, u, u*u, u*u*u);
        glm::vec3 P(Gk * (B * uVec));
        glVertex3fv(&P[0]);
    }
    glEnd();
}

glm::vec3 Spline::computeSplinePoint(glm::mat4 G, glm::mat4 B, float u) {
	glm::vec4 uVec(1.0f, u, u*u, u*u*u);
	return G * (B * uVec);
}

Keyframe Spline::interpolate(float u)
{
    int fcp = floor(u);
    u = u - fcp;
    glm::vec4 uVec(1.0f, u, u*u, u*u*u);

    glm::mat4 Gp;
    glm::mat4 Gq; 
    for(int i = fcp; i < fcp + 4; i++) {
        Gp[i - fcp] = glm::vec4(controlPoints[i], 0.0f); 
        
        glm::quat q = quats[i];
        Gq[i - fcp] = glm::vec4(q.x, q.y, q.z, q.w);
    }

    glm::vec3 pos(Gp * (B * uVec));
    glm::vec4 qVec(Gq * (B * uVec));
    glm::quat q(qVec[3], qVec[0], qVec[1], qVec[2]);

    return Keyframe(pos, q);
}

void Spline::drawSpline(bool drawLine, glm::vec3 color)
{
    if(drawLine) {
        drawCPLine();
    }

    drawSPoints();

    glLineWidth(2.0f);
    glColor3f(color.x, color.y, color.z);

    glBegin(GL_LINE_STRIP);
    for(int cp = 0; cp < uMax; cp++) {

        glm::mat4 Gk; 
        for(int i = 0; i < 4; i++) {
            glm::vec3 point = controlPoints[i + cp];
            Gk[i] = glm::vec4(point.x, point.y, point.z, 0.0f); 
        }

        for(float u = 0.0; u <= 1.0; u += deltaU) {
            glm::vec4 uVec(1.0f, u, u*u, u*u*u);
            glm::vec3 pos(Gk * (B * uVec));
            glVertex3f(pos.x, pos.y, pos.z);
        }
    }

    glEnd();
}
