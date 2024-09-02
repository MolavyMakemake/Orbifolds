#version 330 core

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};


uniform Material material;

uniform vec3 light_color;
uniform vec3 light_normal;
uniform vec3 viewPos;

uniform float scene_ambiant;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    vec3 L;

    // diffuse 
    vec3 normal = normalize(Normal);
    float diffuse = max(dot(normal, light_normal), 0.0);
    vec3 diffuse_color = vec3(texture(material.diffuse, TexCoord));
    //diffuse_color = vec3(1);
    L = (scene_ambiant + diffuse) * diffuse_color;

    // specular
    if (diffuse > 0) {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayVec = normalize(viewDir + light_normal);
        float spec = pow(max(dot(normal, halfwayVec), 0.0), 32);
        L += spec * vec3(texture(material.specular, TexCoord));  
    }

    FragColor = vec4(L * light_color, 1.0);
}