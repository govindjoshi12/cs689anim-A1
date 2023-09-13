#include "Helicopter.h"
#include "Program.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

Helicopter::Helicopter(string RESOURCE_DIR)
{
    body1 = Shape();
    body2 = Shape();
    prop1 = Shape();
    prop2 = Shape();

    body1.loadMesh(RESOURCE_DIR + "helicopter_body1.obj");
    body2.loadMesh(RESOURCE_DIR + "helicopter_body2.obj");
    prop1.loadMesh(RESOURCE_DIR + "helicopter_prop1.obj");
    prop2.loadMesh(RESOURCE_DIR + "helicopter_prop2.obj");

    body1.init();
    body2.init();
    prop1.init();
    prop2.init();

    prop1center = glm::vec3(0.0, 0.4819, 0.0);
    prop2center = glm::vec3(0.6228, 0.1179, 0.1365);
}

Helicopter::~Helicopter()
{

}

void Helicopter::rotatePropeller(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, 
                                double t, glm::vec3 origin, glm::vec3 axis) 
{
    MV->pushMatrix();

    float alpha = std::fmod(t, 1.0f);
    glm::quat q0 = glm::angleAxis((float)(glm::radians(alpha * 360.0)), axis);
	glm::mat4 R0 = glm::mat4_cast(q0);

    MV->translate(origin);
    MV->multMatrix(R0);
    MV->translate(-1.0f * origin);

	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
    MV->popMatrix();
}

void Helicopter::draw(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, 
                    double t, glm::vec3 origin)
{
    MV->pushMatrix();
    MV->translate(origin);

	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
    
    // setting the diffuse component to red (kd) in the shader
    glUniform3f(prog->getUniform("kd"), 1.0f, 0.0f, 0.0f);
	body1.draw(prog);
	
    glUniform3f(prog->getUniform("kd"), 1.0f, 1.0f, 0.0f);
	body2.draw(prog);

    glUniform3f(prog->getUniform("kd"), 0.5f, 0.5f, 0.5f);

    rotatePropeller(prog, MV, t, prop1center, glm::vec3(0.0f, 1.0f, 0.0f));
	prop1.draw(prog);

    rotatePropeller(prog, MV, t, prop2center, glm::vec3(0.0f, 0.0f, 1.0f));
    prop2.draw(prog);

    MV->popMatrix();
}