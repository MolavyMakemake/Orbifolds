#include"Shader.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const char* computeFile) {
	std::string computeCode = get_file_contents(computeFile);

	const char* computeSource = computeCode.c_str();


	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &computeSource, NULL);
	glCompileShader(computeShader);


	ID = glCreateProgram();
	glAttachShader(ID, computeShader);
	glLinkProgram(ID);

	glDeleteShader(computeShader);
}

void Shader::SetLight(Camera& camera, LightingSettings lightingSettings) {
	Activate();
	glUniform1f(Loc("scene_ambiant"), lightingSettings.scene_ambiant);
	glUniform3fv(Loc("light_normal"), 1, (float*)&lightingSettings.light_normal);
	glUniform3fv(Loc("light_color"), 1, (float*)&lightingSettings.light_color);
}

GLuint Shader::Loc(const char* name) {
	return glGetUniformLocation(ID, name);
}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}