#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * vec4(aPos, 1.0);
	TexCoord = aPos;
}