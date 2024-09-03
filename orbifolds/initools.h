#pragma once

#include <iostream>
#include <ini/ini.h>

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