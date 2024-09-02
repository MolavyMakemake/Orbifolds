#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 texCoord;
out vec4 near;
out vec4 far;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = vec4(aPos, 0, 1.0);

	texCoord = (aPos.xy + 1) / 2;

	mat4 inv = inverse(proj * view);

	near = inv * vec4(aPos, -1, 1);
	far = inv * vec4(aPos, 1, 1);
	//view_normal = normalize(far.xyz / far.w - near.xyz / near.w);
}