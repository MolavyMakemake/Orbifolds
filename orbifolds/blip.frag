#version 330 core

uniform sampler2D fbo_texture;

in vec2 texCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(fbo_texture, texCoord);
    FragColor = vec4(abs(texCoord), 0, 1);
}
