#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "HelicopterAnimation/Animation.h"
#include "HelicopterAnimation/SplineSurface.h"

#include <random>

using namespace std;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

int keyPresses[256] = {0}; // only for English keyboards!

shared_ptr<Program> prog;
shared_ptr<Camera> camera;

Animation animation;
Spline spline;
SplineSurface splineSurface;

// RESET CAMERA with C

bool showKeyframesAndSpline = true; // toggle with k
bool helicopterCamera = false; // toggle with space
float sDelta = 1.0f;
float yOffset = 5.0f;

// Increment with =
// Decrement with -
// In range [5, 27]
int numKeyframes = 10;
// Constants
int MAX_KEYFRAMES = 27;
int MIN_KEYFRAMES = 5;

bool pause = false; // No-op. TODO.
bool rHC = true; // toggle with h

// inc/dec in increments of intervalDelta in range [2.0, 6.0]
float keyframeInterval = 5.0f; 

float INTERVAL_DELTA = 1.0f;
float MIN_KEYFRAME_INTERVAL = 2.0f;
float MAX_KEYFRAME_INTERVAL = 6.0f;

float tableDeltaU = 0.2f;
float arcIntervalLength = 1.0f;
bool GQ = true; // toggle with g
auto rd = std::random_device();
long unsigned int seed; // Reset with r

ArcTraversal arc = NALP; // toggle with s

// SPLINE SURFACE PARAMS

// number of Cps on ONE side
bool showSplineSurfaceCps = false; // toggle with M
int SSCP_FACTOR = 3;
// increases/dec with kf interval
int splineSurfaceCps = keyframeInterval * SSCP_FACTOR; 
int segments = 9;
float splineSurfaceMaxHeight = 3.0f;

static void initAnimation() {
	animation = Animation(RESOURCE_DIR, numKeyframes, keyframeInterval, yOffset, seed);
	spline = Spline(animation.getKeyframes(), tableDeltaU, arcIntervalLength, GQ);
	animation.setSpline(spline);
}

static void initSplineSurface() {
	splineSurface = SplineSurface(splineSurfaceCps, keyframeInterval / 2, 
									splineSurfaceMaxHeight, glm::vec3(1, 0, -1), seed, segments);
}

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if(action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_C:
				// Reset camera
				camera = make_shared<Camera>();
				break;
			case GLFW_KEY_K:
				showKeyframesAndSpline = !showKeyframesAndSpline;
				break;
			case GLFW_KEY_S:
				arc = (ArcTraversal)((arc + 1) % AT_COUNT);
				// reset back to origin
				glfwSetTime(0.0);
				break;
			case GLFW_KEY_P:
				pause = !pause;
				break;
			case GLFW_KEY_SPACE:
				helicopterCamera = !helicopterCamera;
				break;
			case GLFW_KEY_H:
				if(helicopterCamera) {
					rHC = !rHC;
				}
				break;
			case GLFW_KEY_M:
				showSplineSurfaceCps = !showSplineSurfaceCps;
				break;
			case GLFW_KEY_COMMA:
				if(keyframeInterval - INTERVAL_DELTA >= MIN_KEYFRAME_INTERVAL) {
					keyframeInterval -= INTERVAL_DELTA;
					splineSurfaceCps = (int)(keyframeInterval * SSCP_FACTOR);
					initAnimation();
					initSplineSurface();
				}
				break;
			case GLFW_KEY_PERIOD:
				if(keyframeInterval + INTERVAL_DELTA <= MAX_KEYFRAME_INTERVAL) {
					keyframeInterval += INTERVAL_DELTA;
					splineSurfaceCps =  (int)(keyframeInterval * SSCP_FACTOR);
					initAnimation();
					initSplineSurface();
				}
				break;
			case GLFW_KEY_EQUAL:
				if(numKeyframes + 1 < MAX_KEYFRAMES) {
					numKeyframes++;
					initAnimation();
				}
				break;
			case GLFW_KEY_MINUS:
				if(numKeyframes - 1 >= MIN_KEYFRAMES) {
					numKeyframes--;
					initAnimation();
				}
				break;
			case GLFW_KEY_G:
				GQ = !GQ;
				initAnimation(); // Technically just need to rebuild table
				break;
			case GLFW_KEY_R:
				seed = rd();
				initAnimation();
				initSplineSurface();
				break;
		}
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyPresses[key]++;
}

static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved(xmouse, ymouse);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = mods & GLFW_MOD_SHIFT;
		bool ctrl  = mods & GLFW_MOD_CONTROL;
		bool alt   = mods & GLFW_MOD_ALT;
		camera->mouseClicked(xmouse, ymouse, shift, ctrl, alt);
	}
}

static void init()
{
	GLSL::checkVersion();
	
	// Set background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);

	keyPresses[(unsigned)'c'] = 1;
	
	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	prog->setVerbose(true);
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addUniform("lightPos");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->setVerbose(false);
	
	camera = make_shared<Camera>();
	
	seed = rd();
	initAnimation();
	initSplineSurface();

	// Initialize time.
	glfwSetTime(0.0);
	
	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);
}

void render()
{
	// Update time.
	double t = glfwGetTime();
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	// Use the window size for camera.
	glfwGetWindowSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyPresses[(unsigned)'c'] % 2) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyPresses[(unsigned)'z'] % 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	P->pushMatrix();
	P->loadIdentity();

	MV->pushMatrix();
	MV->loadIdentity();

	camera->applyProjectionMatrix(P);

	// Apply camera transforms
	prog->bind();

	splineSurface.draw(prog);

	camera->applyViewMatrix(MV);
	animation.render(prog, P, MV, camera, t, showKeyframesAndSpline, arc, helicopterCamera, rHC);

	prog->unbind();

	// Draw the frame and the grid with OpenGL 1.x (no GLSL)
	
	// Setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(glm::value_ptr(P->topMatrix()));
	
	// Setup the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(glm::value_ptr(MV->topMatrix()));
	
	// Draw Spline
	if(showKeyframesAndSpline) {
		spline.drawSpline();
	}

	// Draw Spline Surface Points
	if(showSplineSurfaceCps) {
		splineSurface.drawCps();
	}

	// Draw frame
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();
	glLineWidth(1);
	
	// Draw grid
	float gridSizeHalf = 20.0f;
	int gridNx = 40;
	int gridNz = 40;
	glLineWidth(1);
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_LINES);
	for(int i = 0; i < gridNx+1; ++i) {
		float alpha = i / (float)gridNx;
		float x = (1.0f - alpha) * (-gridSizeHalf) + alpha * gridSizeHalf;
		glVertex3f(x, 0, -gridSizeHalf);
		glVertex3f(x, 0,  gridSizeHalf);
	}
	for(int i = 0; i < gridNz+1; ++i) {
		float alpha = i / (float)gridNz;
		float z = (1.0f - alpha) * (-gridSizeHalf) + alpha * gridSizeHalf;
		glVertex3f(-gridSizeHalf, 0, z);
		glVertex3f( gridSizeHalf, 0, z);
	}
	glEnd();
	
	// Pop modelview matrix
	glPopMatrix();
	
	// Pop projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	// Pop stacks
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");
	
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		if(!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
			// Render scene.
			render();
			// Swap front and back buffers.
			glfwSwapBuffers(window);
		}
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
