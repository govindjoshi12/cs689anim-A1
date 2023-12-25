#include "SplineSurface.h"
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// spline patch
SplineSurface::SplineSurface(int numSideCps, float singlePatchLen, float maxHeight, 
                            glm::vec3 center, int seed, int segments) {

    numSideCps = max(numSideCps, 4); // Minimum 4 control points
    singlePatchLen = max(singlePatchLen, 1.0f); // Minimum 1.0f singlePatchLen length
    maxHeight = max(maxHeight, 1.0f); // Min height 1.0f
    segments = max(segments, 1); // At least one segment

    this->numSideCps = numSideCps;
    float sideLength = singlePatchLen * (numSideCps - 1);

    float halfSideLen = sideLength / 2.0f;
    float interval = sideLength / (float)(numSideCps);
    glm::vec3 topLeft(-1 * halfSideLen, 0, halfSideLen);

    // Control points form a square around the provided origin
    srand(seed);
    for(int i = 0; i < numSideCps; i++) {
        vector<glm::vec3> row;
        float zSub = interval * i;
        for(int j = 0; j < numSideCps; j++) {
            float xAdd = interval * j;
    		float y = (float)(((double) rand() / (RAND_MAX)) * maxHeight);

            row.push_back(glm::vec3(topLeft.x + xAdd, y, topLeft.z - zSub) + center);
        }
        cps.push_back(row);
    }

	glm::mat4 b_spline;
	b_spline[0] = glm::vec4(1, 4, 1, 0);
	b_spline[1] = glm::vec4(-3, 0, 3, 0);
	b_spline[2] = glm::vec4(3, -6, 3, 0);
	b_spline[3] = glm::vec4(-1, 3, -3, 1);
    B = (1.0f / 6.0f) * b_spline;

    int totalQuadrants = pow(numSideCps - 3, 2);
    for(int i = 0; i < totalQuadrants; i++) {
        createSurfaceTrianlges(i, segments);
    }
    initMesh();
}

SplineSurface::~SplineSurface() {

}

glm::vec3 SplineSurface::computePoint(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B) {
    glm::vec4 uVec(1.0f, u, u*u, u*u*u);
    glm::vec4 vVec(1.0f, v, v*v, v*v*v);

    // 4x1 * 4x4 * 4x4 * 4x4 * 1x4
    float x = glm::dot(vVec, (glm::transpose(B) * (Gx * (B * uVec))));
    float y = glm::dot(vVec, (glm::transpose(B) * (Gy * (B * uVec))));
    float z = glm::dot(vVec, (glm::transpose(B) * (Gz * (B * uVec))));

    return glm::vec3(x, y, z);
}

glm::vec3 SplineSurface::computeNormal(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B) {
    glm::vec4 uVec(1.0f, u, u*u, u*u*u);
    glm::vec4 vVec(1.0f, v, v*v, v*v*v);
    glm::vec4 uPrime(0.0f, 1.0f, 2*u, 3*u*u);
    glm::vec4 vPrime(0.0f, 1.0f, 2*v, 3*v*v);

    // 4x1 * 4x4 * 4x4 * 4x4 * 1x4
    float ux = glm::dot(vVec, (glm::transpose(B) * (Gx * (B * uPrime))));
    float uy = glm::dot(vVec, (glm::transpose(B) * (Gy * (B * uPrime))));
    float uz = glm::dot(vVec, (glm::transpose(B) * (Gz * (B * uPrime))));
    glm::vec3 dU = glm::vec3(ux, uy, uz);

    float vx = glm::dot(vPrime, (glm::transpose(B) * (Gx * (B * uVec))));
    float vy = glm::dot(vPrime, (glm::transpose(B) * (Gy * (B * uVec))));
    float vz = glm::dot(vPrime, (glm::transpose(B) * (Gz * (B * uVec))));
    glm::vec3 dV = glm::vec3(vx, vy, vz);

    return glm::cross(dU, dV);
}

void SplineSurface::pushPointAndNormal(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B) {
    
    glm::vec3 p = computePoint(u, v, Gx, Gy, Gz, B);
    glm::vec3 n = computeNormal(u, v, Gx, Gy, Gz, B);

    // [1, 1, 1], [2, 3, 4], [1, 2, 3]
    // 1, 1, 1, 2, 3, 4, 1, 2, 3
    posBuf.push_back(p.x);
    posBuf.push_back(p.y);
    posBuf.push_back(p.z);

    norBuf.push_back(n.x);
    norBuf.push_back(n.y);
    norBuf.push_back(n.z);
}


// Quadrant depends on number of control points
void SplineSurface::createSurfaceTrianlges(int quadrant, int segments) {

    int firstPointRow = (int)(quadrant / (numSideCps - 3));
    int firstPointCol = quadrant % (numSideCps - 3);

    glm::mat4 Gx;
    glm::mat4 Gy;
    glm::mat4 Gz;
    for(int i = firstPointRow; i < firstPointRow + 4; i++) {
        for(int j = firstPointCol; j < firstPointCol + 4; j++) {
            // glm matrices are col major
            glm::vec p = cps[i][j];
            Gx[j - firstPointCol][i - firstPointRow] = p.x;
            Gy[j - firstPointCol][i - firstPointRow] = p.y;
            Gz[j - firstPointCol][i - firstPointRow] = p.z;
        }
    }

    float delta = 1.0f / (float)(segments);

    for(float u = 0; u <= 1 - delta; u += delta) {
        for(float v = 0; v <= 1 - delta; v += delta) {
            float uRight = u + delta;
            float vTop = v + delta;

            pushPointAndNormal(u, v, Gx, Gy, Gz, B);
            pushPointAndNormal(uRight, v, Gx, Gy, Gz, B); 
            pushPointAndNormal(u, vTop, Gx, Gy, Gz, B); 

            pushPointAndNormal(uRight, v, Gx, Gy, Gz, B);
            pushPointAndNormal(uRight, vTop, Gx, Gy, Gz, B);
            pushPointAndNormal(u, vTop, Gx, Gy, Gz, B); 

        }
    }
}

void SplineSurface::initMesh() {
    // Modified from Shape.cpp

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
    glGenBuffers(1, &norBufID);
    glBindBuffer(GL_ARRAY_BUFFER, norBufID);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	
	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Immediate mode
void SplineSurface::drawCps() {
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for(vector<glm::vec3> points : cps) {
        for(glm::vec3 P : points) {
            glVertex3fv(&P[0]);
        }
    }
    glEnd();
}

void SplineSurface::draw(const shared_ptr<Program> prog) {

    // Copied and modified from Shape::draw
    glUniform3f(prog->getUniform("kd"), 0.125f, 0.78f, 0.839f);

	// Bind position buffer
	int h_pos = prog->getAttribute("aPos");
	glEnableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	int h_nor = prog->getAttribute("aNor");
	if(h_nor != -1 && norBufID != 0) {
		glEnableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}
	
	// Draw
	int count = (int)posBuf.size()/3; // number of indices to be rendered
	glDrawArrays(GL_TRIANGLES, 0, count);
	
	// Disable and unbind
	if(h_nor != -1) {
		glDisableVertexAttribArray(h_nor);
	}
	glDisableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}