#include "Camera.h"
#include <iostream>
#include <glm/gtx/euler_angles.hpp>

void Camera::Bake(int width, int height) {

	glm::vec4 pos = glm::eulerAngleYX(euler.x, euler.y) * glm::vec4(0, 0, radius, 0);

	view = glm::lookAt(glm::vec3(pos), target, glm::vec3(0, 1, 0));
	proj = glm::perspective(glm::radians(fov / 2), (float)width / height, nearClippingPlane, farClippingPlane);
	mat = proj * view;
}

void Camera::rotate(glm::vec3 _euler) {
	euler.x += _euler.x;
	euler.y = glm::clamp(euler.y - _euler.y, -1.4f, 1.4f);
}