#version 330 core

uniform sampler2D tex;

in vec3 normal;
in vec2 uv;
in vec2 value;

out vec4 FragColor;

void main()
{
    FragColor = texture(tex, uv) * (0.6 * max(normal.z, 0) + 0.4);

    //float t = clamp(value.x / 5, .3, 1) - 0.3;
    //float s = (1 - t) * exp(-t);
    //FragColor += normal;
}