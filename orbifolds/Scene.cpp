#include "Scene.h"
#include <imgui.h>

void Scene::Add(Model* model, const char* name) {
	objects.push_back(SceneObject(name, model));
}

void Scene::Draw(Shader& shader) {
	for (int i = 0; i < objects.size(); i++) {
		(*objects[i].model).Draw(shader, camera);
	}
}

void Scene::DrawGUI() {
	ImGui::Begin("Scene");
	ImGui::DragFloat("Longitude", &longitude, 0.01f);
	ImGui::DragFloat("Latitude", &latitude, 0.01f, -90, 90);

	if (longitude > 180) longitude -= 360;
	else if (longitude < -180) longitude += 360;


	if (ImGui::BeginCombo("##combo", name))
	{
		for (int n = 0; n < scenes.size(); n++)
		{
			bool is_selected = (name == scenes[n].name);
			//if (ImGui::Selectable(scenes[n].name, is_selected))
				//current_item = items[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	ImGui::End();
}

void Scene::Save(Scene &scene) {
	std::ofstream file;
	file.open(scene.path);

	file << "[Scene]" << "[" << scene.name << "]\n";
	file << "longitude " << scene.longitude << "\n";
	file << "latitude " << scene.latitude << "\n\n";

	file << "[Lighting]\n";
	file << "color " << (float*)&scene.lightingSettings.light_color << "\n";
	file << "ambiant " << (float*)&scene.lightingSettings.scene_ambiant << "\n\n";

	file << "[Camera]\n";
	file << "fov=" << scene.camera.fov << "\n";
	file << "near clipping plane=" << scene.camera.nearClippingPlane << "\n";
	file << "far clipping plane=" << scene.camera.farClippingPlane << "\n\n";

	for (int i = 0; i < scene.objects.size(); i++) {
		SceneObject& obj = scene.objects[i];
		//file << "[Model]" << "[" << obj.name << "]\n";
		//file << "path " << obj.model.getPath() << "\n";
		//file << "position " << (float*)&obj.model.position << "\n";
		//file << "rotation " << (float*)&obj.model.rotation << "\n";
		//file << "scale " << (float*)&obj.model.scale << "\n\n";
	}

	file.close();
}

void Scene::loadScene(const char* path) {
	std::ifstream file;
	file.open(path);

	path = path;
	name = "Sample Scene";

	if (file.is_open()) {

		std::string line;
		std::string header;
		while (std::getline(file, line)) {
			if (std::strcmp(line.c_str(), "") == 0) // Skip empty lines
				continue;

			// Look for header
			size_t h0 = line.find_first_of('['), h1 = line.find_first_of(']', h0);
			if (h0 != std::string::npos && h1 != std::string::npos) {
				header = line.substr(h0, h1);
				std::cout << header << std::endl;

				continue;
			}

			// Get type
			size_t t0 = line.find_first_not_of(' '), t1 = line.find_first_of('=', t0);
			if (t0 == std::string::npos || t1 == std::string::npos || t1 == line.length())
				continue;

			std::string type = line.substr(t0, t1);
			std::string arg = line.substr(t1 + 1, line.length());

			// Remove padding
			type = header + type.substr(0, type.find_last_not_of(' ') + 1);
			arg = arg.substr(arg.find_first_not_of(' '), arg.find_last_not_of(' ') + 1);

			std::cout << header << type << ", " << arg << std::endl;

			if (header != "" && map.count(type) != 0) {
				//float* t = (float*)&arg;
				//std::cout << *t << std::endl;
				//map[header][type] = (float*)&arg;
			}
		}

		file.close();
	}
	else
		std::cout << "Could not find " << path << std::endl;
}