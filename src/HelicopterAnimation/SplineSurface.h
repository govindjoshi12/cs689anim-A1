#include <vector>
#include <glm/glm.hpp>
#include "../Program.h"
#include <memory>

using namespace std;

class SplineSurface
{
public:
    SplineSurface() {};
    SplineSurface(int numSideCps, float singlePatchLen, float maxHeight, 
                glm::vec3 center, int seed, int segments);
    virtual ~SplineSurface();
    void drawCps();
    void draw(const shared_ptr<Program> prog);

private:
    int numSideCps;
    vector<vector<glm::vec3>> cps;
    glm::mat4 B;

    // Triangles
    std::vector<float> posBuf;
	std::vector<float> norBuf;
	unsigned posBufID;
	unsigned norBufID;

    // Methods
    glm::vec3 computePoint(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B);
    glm::vec3 computeNormal(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B);
    void pushPointAndNormal(float u, float v, glm::mat4 Gx, glm::mat4 Gy, glm::mat4 Gz, glm::mat4 B);
    void createSurfaceTrianlges(int quadrant, int segments);
    void initMesh();
};