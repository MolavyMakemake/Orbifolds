#version 330 core

uniform sampler2D tex;

in vec3 normal;
in vec2 uv;
in vec2 value;

out vec4 FragColor;

void main()
{
    FragColor = texture(tex, uv) * (max(normal.z, 0) + 0.1);
    //FragColor.x = abs(value.x);
}