#include "Spline.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

Spline::Spline()
{
}

Spline::Spline(vector<Keyframe> keyframes)
{
    int numCps = (int)(keyframes.size());
    for (int i = 0; i < numCps; i++)
    {
        controlPoints.push_back(keyframes[i].locvec());
    }

    B[0] = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f);
    B[1] = glm::vec4(-1.0f, 0.0f, 1.0f, 0.0f);
    B[2] = glm::vec4(2.0f, -5.0f, 4.0f, 1.0f);
    B[3] = glm::vec4(-1.0f, 3.0f, -3.0f, 1.0f);
    B = 0.5f * B;

    deltaU = 0.01;
    uMax = numCps - 3;
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

    glLineWidth(3.0f);
    glColor3f(color.x, color.y, color.z);

    glBegin(GL_LINE_STRIP);
    for(int cp = 0; cp < uMax; cp++) {

        glm::mat4 Gk; 
        // cout << "---\n";
        for(int i = 0; i < 4; i++) {
            glm::vec3 point = controlPoints[i + cp];
            Gk[i] = glm::vec4(point, 0.0f); 
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
