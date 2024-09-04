#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aValue;

out vec3 normal;
out vec2 uv;
out vec2 value;

uniform vec3 worldPos;
uniform mat4 camera;


void main()
{
	//vec4 worldPos = model * vec4(aPosition, 1.0);
	gl_Position = camera * vec4(aPosition, 1);

	normal = aNormal;
	uv = aTexCoord;
	value = aValue;
}