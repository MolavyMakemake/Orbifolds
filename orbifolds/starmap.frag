#version 330 core

uniform sampler2D fbo_texture;
uniform sampler2D depth_texture;

in vec2 texCoord;
in vec3 viewDir;

out vec4 FragColor;


void main(){
	FragColor = texture(fbo_texture, texCoord);
}