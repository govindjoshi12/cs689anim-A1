#include "Spline.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

Spline::Spline()
{
}

Spline::Spline(vector<glm::vec3> cps)
{
    controlPoints = cps;

	glm::mat4 catmullRom;
	catmullRom[0] = glm::vec4(0, 2, 0, 0);
	catmullRom[1] = glm::vec4(-1, 0, 1, 0);
	catmullRom[2] = glm::vec4(2, -5, 4, -1);
	catmullRom[3] = glm::vec4(-1, 3, -3, 1);
    B = 0.5f * catmullRom;

    deltaU = 0.01;
    uMax = (int)(controlPoints.size()) - 3;
}

Spline::~Spline()
{
}

void Spline::drawCPLine()
{
    glBegin(GL_LINE_STRIP);
    for(glm::vec3 i : controlPoints) {
        glVertex3f(i.x, i.y, i.z);
    }
    glEnd();
}

glm::vec3 Spline::splineFunc(float u)
{
    glm::mat4 Gk; 
    int fcp = floor(u);
    u = u - fcp;

    for(int i = fcp; i < fcp + 4; i++) {
        Gk[i - fcp] = glm::vec4(controlPoints[i], 0.0f); 
    }

    glm::vec4 uVec(1.0f, u, u*u, u*u*u);

    return glm::vec3(Gk * (B * uVec));
}

void Spline::drawSpline(bool drawLine, glm::vec3 color)
{
    if(drawLine) {
        drawCPLine();
    }

    glColor3f(color.x, color.y, color.z);

    glBegin(GL_LINE_STRIP);
    for(int cp = 0; cp < uMax; cp++) {

        glm::mat4 Gk; 
        // cout << "---\n";
        for(int i = 0; i < 4; i++) {
            glm::vec3 point = controlPoints[i + cp];
            Gk[i] = glm::vec4(point.x, point.y, point.z, 0.0f); 
            // cout << point.x << ", " << point.y << ", " << point.z << "\n";
        }

        for(float u = 0.0; u <= 1.0; u += deltaU) {
            glm::vec4 uVec(1.0f, u, u*u, u*u*u);
            glm::vec3 pos(Gk * (B * uVec));
            glVertex3f(pos.x, pos.y, pos.z);
        }
    }

    glEnd();
}
