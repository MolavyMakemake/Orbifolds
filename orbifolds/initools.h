#pragma once

#include <iostream>
#include <ini/ini.h>
#include "PostProccessing.h"

#define DEFAULT_FONTSIZE	14

template<typename T>
inline bool ini_readInt(mINI::INIMap<std::string>& map, std::string key, T* var, int base = 10) {
	if (map[key].empty())
		return false;
	
	char* p;
	auto _var = std::strtoul(map[key].c_str(), &p, base);
	if (*p) {
		std::cout << "Warning! Could not read " << key << " \"" + map[key] << "\"\n";
		return false;
	}

	*var = _var;
	return true;
}

inline std::string ini_getFilename(std::string filename, std::string extension = "") {
	if (filename.size() < extension.size() || filename.compare(filename.size() - extension.size(), extension.size(), extension))
		filename += extension;
	return "../appdata/" + filename;
}

inline bool ini_readFont(mINI::INIMap<std::string>& map, std::string key, std::string* ttf, int* fontsize) {
	if (map[key].empty())
		return false;
	
	size_t _off = map[key].find_first_of(',');

	if (_off == std::string::npos) {
		std::cout << "Warning! Font size not specified.\n";
		*ttf = map[key];
		*fontsize = DEFAULT_FONTSIZE;
		return true;
	}

	char* p = nullptr;
	*ttf = ini_getFilename(std::string(map[key], 0, _off));
	*fontsize = std::strtoul(std::string(map[key], _off + 1).c_str(), &p, 10);
	
	if (*p || !*fontsize) {
		std::cout << "Warning! Could not read font size.\n";
		*fontsize = DEFAULT_FONTSIZE;
	}

	return true;
}

inline bool ini_readFloat(std::string str, float* var) {
	if (str.empty())
		return false;
	
	char* p;
	float _var = std::strtof(str.c_str(), &p);
	if (*p)
		return false;
	

	*var = _var;
	return true;
}

inline bool ini_readFloat(mINI::INIMap<std::string>& map, std::string key, float* var) {
	if (!ini_readFloat(map[key], var)) {
		std::cout << "Warning! Could not read " << key << " \"" + map[key] << "\"\n";
		return false;
	}

	return true;
}

inline bool ini_readVec3(mINI::INIMap<std::string>& map, std::string key, glm::vec3* var) {
	size_t _off1 = map[key].find_first_of(',');
	size_t _off2 = map[key].find_first_of(',', _off1 + 1);
	if (_off2 == std::string::npos) {
		std::cout << "Warning! Could not read vector " << key << std::endl;
		return false;
	}

	glm::vec3 _var;
	bool isSuccess = true;
	isSuccess |= ini_readFloat(std::string(map[key], 0, _off1), &_var.x);
	isSuccess |= ini_readFloat(std::string(map[key], _off1 + 1, _off2 - _off1 - 1), &_var.y);
	isSuccess |= ini_readFloat(std::string(map[key], _off2 + 1), &_var.z);

	if (!isSuccess) {
		std::cout << "Warning! Could not read vector " << key << std::endl;
		return false;
	}

	*var = _var;
	return true;
}

inline void ini_readPlume(mINI::INIMap<std::string>& map, plumeSettings_t* plume_settings, std::vector<plume_t>* plume_v) {
	ini_readVec3(map, "position", &plume_settings->position);
	ini_readVec3(map, "normal", &plume_settings->normal);
	ini_readVec3(map, "color", &plume_settings->color);
	ini_readFloat(map, "radius", &plume_settings->cc_radius);

	if (!map["load"].empty()) {
		mINI::INIFile plume_f(ini_getFilename(map["load"]));
		mINI::INIStructure plume_v_ini;

		if (!plume_f.read(plume_v_ini))
			return;

		for (auto plume_ini : plume_v_ini) {
			plume_t& plume = plume_v->emplace_back();
			plume.id = plume_ini.first;
			
			ini_readFloat(plume_ini.second, "threshold",	&plume.threshold);
			ini_readFloat(plume_ini.second, "velocity",		&plume.velocity);
			ini_readFloat(plume_ini.second, "length",		&plume.length);
			ini_readFloat(plume_ini.second, "slope",		&plume.slope);
			ini_readFloat(plume_ini.second, "noise",		&plume.noise);
			ini_readFloat(plume_ini.second, "roughness",	&plume.roughness);
			ini_readFloat(plume_ini.second, "seperation",	&plume.seperation);
		}
	}
}

inline void ini_readCamera(mINI::INIMap<std::string>& map, Camera* camera) {
	ini_readVec3(map, "position", &camera->position);
	ini_readVec3(map, "target", &camera->target);

	ini_readFloat(map, "near", &camera->nearClippingPlane);
	ini_readFloat(map, "far", &camera->farClippingPlane);
	ini_readFloat(map, "exposure", &camera->exposure);
	ini_readFloat(map, "gamma", &camera->gamma);
}

inline std::string to_string(glm::vec3 vec) {
	return std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z);
}

inline void ini_writePlume(mINI::INIMap<std::string>& map, plumeSettings_t* plume_settings, std::vector<plume_t>* plume_v) {
	map["position"] = to_string(plume_settings->position);
	map["normal"] = to_string(plume_settings->normal);
	map["color"] = to_string(plume_settings->color);

	if (map["load"].empty())
		map["load"] = "rendering/plume.ini";

	mINI::INIFile plume_f(ini_getFilename(map["load"]));
	mINI::INIStructure plume_v_ini;

	for (plume_t& plume : *plume_v) {
		plume_v_ini[plume.id]["threshold"]	= std::to_string(plume.threshold);
		plume_v_ini[plume.id]["velocity"]	= std::to_string(plume.velocity);
		plume_v_ini[plume.id]["length"]		= std::to_string(plume.length);
		plume_v_ini[plume.id]["slope"]		= std::to_string(plume.slope);
		plume_v_ini[plume.id]["noise"]		= std::to_string(plume.noise);
		plume_v_ini[plume.id]["roughness"]	= std::to_string(plume.roughness);
		plume_v_ini[plume.id]["seperation"]	= std::to_string(plume.seperation);
	}

	plume_f.generate(plume_v_ini, true);
}

inline void ini_writeCamera(mINI::INIMap<std::string>& map, Camera* camera) {
	map["position"] = to_string(camera->position);
	map["target"] = to_string(camera->target);

	map["near"] = std::to_string(camera->nearClippingPlane);
	map["far"] = std::to_string(camera->farClippingPlane);
	map["exposure"] = std::to_string(camera->exposure);
	map["gamma"] = std::to_string(camera->gamma);
}