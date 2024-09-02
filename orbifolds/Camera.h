#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Camera {
	float farClippingPlane = 100.0f;
	float nearClippingPlane = 1.f;
	
	float exposure = 1;
	float gamma = 1;

	glm::vec3 position = glm::vec3(0, 0, 5);
	glm::vec3 target = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);

	glm::mat4 view;
	glm::mat4 proj;

	float fov = 90.0f;

	void Bake(int width, int height);
};