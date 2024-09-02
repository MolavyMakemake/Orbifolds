#include "Camera.h"
#include <iostream>

void Camera::Bake(int width, int height) {
	view = glm::lookAt(position, target, up);
	proj = glm::perspective(glm::radians(fov / 2), (float)width / height, nearClippingPlane, farClippingPlane);
}