#pragma once

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include "glm/glm.hpp"
#include "Camera.h"

std::string get_file_contents(const char* filename);

struct LightingSettings {
	glm::vec3 light_normal = glm::vec3(1.0f, -0.02f, 0.2f);
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
	float scene_ambiant = 1.02f;
};

struct Material {
	glm::vec3 albedo = glm::vec3(1.0f, 0.0f, 1.0f);
	float diffuse;
	float specular;
	float shininess;
};

class Shader {
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader(const char* computeFile);
	Shader(){}

	void SetLight(Camera& camera, LightingSettings lightingSettings);
	void Activate();
	void Delete();

	GLuint Loc(const char* name);
};