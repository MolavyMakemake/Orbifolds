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

enum SHAPE_ {
	SHAPE_2222, SHAPE_333, SHAPE_442, SHAPE_632
};

enum MESH_ {
	MESH_P1, MESH_PM, MESH_PG, MESH_CM,
	MESH_P2, MESH_PMM, MESH_PMG, MESH_PGG, MESH_CMM,
	MESH_P3, MESH_P3M1, MESH_P31M,
	MESH_P4, MESH_P4M, MESH_P4G,
	MESH_P6, MESH_P6M
};

static const char* mesh_id[] = {
	"p1", "pm", "pg", "cm", 
	"p2", "pmm", "pmg", "pgg", "cmm",
	"p3", "p3m1", "p31m",
	"p4", "p4m", "p4g",
	"p6", "p6m"
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

	glm::ivec2 resolution;

private:
	GLuint VAO, VBO, EBO;
	void Generate();

	void computeMesh(GLuint res_x, GLuint res_y, SHAPE_ shape,
		std::vector<GLuint>& identify, std::vector<GLuint>& edge, float (*_len)(float) = _len_1);

	static inline float _len_1(float y) { return 1; }

	void init2222(GLuint res_x, GLuint res_y,
		std::vector<GLuint>& identify, std::vector<GLuint>& edge, float (*_len)(float));
	void init333(GLuint res,
		std::vector<GLuint>& identify, std::vector<GLuint>& edge);
	void init442(GLuint res,
		std::vector<GLuint>& identify, std::vector<GLuint>& edge, 
		float _shift = 0, float _stretch = 1);
	void init632(GLuint res,
		std::vector<GLuint>& identify, std::vector<GLuint>& edge);
};