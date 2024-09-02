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
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	glm::mat4 matrix;

	Material material;

	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures);
	
	void RecalculateNormals();

	void Draw(Shader& shader, Camera& camera);
	void Delete();

private:
	GLuint VAO, VBO, EBO;

	void Generate();
};