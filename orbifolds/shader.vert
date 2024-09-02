#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform vec3 worldPos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;


void main()
{
	//vec4 worldPos = model * vec4(aPosition, 1.0);
	vec4 worldPos = vec4(aPosition, 1.0);
	FragPos = vec3(worldPos);
	gl_Position = proj * view * worldPos;

	Normal = aNormal;
	TexCoord = aTexCoord;
}