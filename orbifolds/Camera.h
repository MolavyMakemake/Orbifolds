#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Camera {
	float farClippingPlane = 100.0f;
	float nearClippingPlane = 1.f;
	
	float exposure = 1;
	float gamma = 1;

	glm::vec3 euler = glm::vec3(0, 0, 0);
	glm::vec3 target = glm::vec3(0);

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 mat;

	float fov = 90.0f;

	void Bake(int width, int height);
	void rotate(glm::vec3 _euler);
};