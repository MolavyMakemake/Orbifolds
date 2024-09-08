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

const float pi = 6.2831853f;
const float phi = 1.618034f;

struct shape_t {
	float l1;
	float l2;
	float w;
};

struct param_t {
	float k = 2;
	float dampening = 1.f;
	float smoothing = 1.f;
	float explode = 0;
	float pressure = 1;
	float stretch = 1;
};

enum SHAPE_ {
	SHAPE_2222, SHAPE_333, SHAPE_442, SHAPE_662, SHAPE_632
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
	std::vector<GLuint> edge;

	std::vector<shape_t> shapes;
	std::vector<glm::vec3> velocity;

	Texture texture;
	param_t par;

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
		std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float) = _pos_id);

	static inline glm::vec3 _pos_id(float x, float y) { return glm::vec3(2.f * x - 1.f, 2.f * y - 1, 0.f); }

	void init2222(GLuint res_x, GLuint res_y, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float));
	void init333(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float));
	void init442(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float));
	void init632(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float));
	void init662(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float));

	void _triangulate(std::vector<GLuint>& identify, GLuint i, GLuint j, GLuint k);
};