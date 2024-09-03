#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 uv;

uniform vec3 worldPos;
uniform mat4 camera;


void main()
{
	//vec4 worldPos = model * vec4(aPosition, 1.0);
	gl_Position = camera * vec4(aPosition, 1);

	uv = aTexCoord;
}