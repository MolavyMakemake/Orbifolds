#pragma once

#include<glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include<string>
#include<vector>

#include"Shader.h"
#include "Camera.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec2 value;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

GLuint textureFromFile(const char* path, const std::string& directory, bool gamma=false);

struct spring_t {
	GLuint i;
	GLuint j;
	float length;
};

enum MESH_ {
	MESH_RECTANGLE,
	MESH_P1,
	MESH_PM,
	MESH_CM,
	MESH_P4,
};

static const char* mesh_id[] = {
	"rectangle", "p1", "pm", "cm", "p4"
};

class Mesh {
public:
	MESH_ domain;
	clock_t iteration;

	std::vector<Vertex> vertices;
	std::vector<GLuint> triangles;

	std::vector<spring_t> springs;
	std::vector<spring_t> edgeSprings;

	std::vector<float> area;

	Texture texture;

	Mesh(MESH_ domain, GLuint res_x, GLuint res_y);
	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures);
	
	void Update();

	void Draw();
	void Delete();
	void Reset();

	void computeDomain();
	void iterate(float dt);

	GLuint res_x, res_y;

private:
	GLuint VAO, VBO, EBO;

	void init_rectangle(GLuint res_x, GLuint res_y, std::vector<GLuint>& identify, std::vector<GLuint>& edge);
	void Generate();
};