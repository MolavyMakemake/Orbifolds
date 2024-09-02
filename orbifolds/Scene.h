#pragma once
#include <string>
#include <vector>
#include <map>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"


class Scene {
private:
	struct SceneObject {
		SceneObject(std::string name, Model* model) {
			this->model = model;
			this->name = name;
		}
		std::string name;
		Model* model;
	};
	struct ScenePtr {
		const char* name;
		const char* path;
	};

public:
	void Draw(Shader& shader); // TODO : add support for multiple shaders
	void DrawGUI();

	void Add(Model* model, const char* name = "object");
	void Remove();

	static Scene Load(const char* path) {
		Scene scene;
		scene.loadScene(path);
		return scene;
	}
	static void Save(Scene& scene);

	glm::vec3 position = glm::vec3(0);
	const char* name = "scene a";

	Camera camera;
	LightingSettings lightingSettings;
	std::vector<SceneObject> objects;

private:
	const char* path = "scenes/scene a.txt";
	std::vector<ScenePtr> scenes;

	float longitude = -80.6077f;
	float latitude = 28.3922f;

	std::map<std::string, float*> map = {
		{ "[Camera]near clipping plane", &camera.nearClippingPlane },
		{ "[Camera]far clipping plane", &camera.farClippingPlane },
		{ "[Camera]position", (float*)&camera.position}
	};

	void loadScene(const char* path);
};