#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Cubemap.h"

#define PLUME_STATE_LEAK    0
#define PLUME_STATE_PRE     1
#define PLUME_STATE_HOT     2

struct plume_t {
    std::string id;

    float threshold;
    float velocity;
    float length;
    float slope;
    float noise;
    float roughness;
    float seperation;
};

struct plumeSettings_t {
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 normal = glm::vec3(0, -1, 0);
    glm::vec3 color = glm::vec3(1, 1, 1);

    float cc_pressure = 10;
    float cc_radius = 0.5;

    int state = 0;
    clock_t t_clk = 0;
};

class PostProcessing {
public:
    PostProcessing();

    void Draw(Shader& shader);
    void Draw(Shader& shader, Scene& scene);
    void Draw(Shader& shader, Scene& scene, plumeSettings_t& plumeSettings, plume_t& plume, unsigned int perlin);
	void Draw(Shader& shader, Scene& scene, Cubemap& cubemap);

private:
	GLuint VAO, VBO;
    float daytimeFade = 10;
};

const float quadVertices[] = {
     1.0f,  1.0f,
     1.0f, -3.0f,
    -3.0f,  1.0f
};