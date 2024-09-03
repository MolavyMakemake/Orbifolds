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
	glm::vec2 texCoords;
	glm::vec2 value;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

enum MESH_ {
	MESH_RECTANGLE
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> triangles;
	Texture texture;

	glm::mat4 matrix;

	Material material;

	Mesh(MESH_ domain, int res_x, int res_y);
	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures);
	
	void RecalculateNormals();

	void Draw();
	void Delete();

private:
	GLuint VAO, VBO, EBO;

	void init_rectangle(int res_x, int res_y);
	void Generate();
};