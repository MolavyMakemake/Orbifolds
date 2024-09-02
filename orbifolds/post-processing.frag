#version 330 core

uniform sampler2D fbo_texture;
uniform sampler2D depth_texture;
uniform sampler2D perlin_texture;

uniform vec3 perlin_sample;

in vec2 texCoord;
in vec4 near;
in vec4 far;

out vec4 FragColor;

// Light
uniform vec3 light_color;
uniform vec3 light_normal;
uniform vec3 view_position;
uniform float scene_ambiant;

// Camera
uniform float exposure;
uniform float gamma;

// Plume
uniform float cc_radius;
uniform float cc_pressure;

uniform vec3 plume_position;
uniform vec3 plume_normal;
uniform vec3 plume_color;

uniform float plume_roughness;
uniform float plume_seperation;
uniform float plume_noise;
uniform float plume_length;
uniform float plume_slope;
uniform float plume_t;

float plume_radius(float t) {
    return cc_radius * 1 + plume_slope * t;
}

float plume_radiusScalar(float t) {
    float sd_seperation = max(0.67 * cc_radius * sqrt(cc_pressure), 0.001);
    return plume_seperation * abs(sin(6.28 / sd_seperation * t));
}

bool plume_intersect(vec3 view_normal, inout float t, inout float l, inout float r) {
    vec3 perp = cross(view_normal, plume_normal);
    t = dot(cross(plume_position - view_position, plume_normal), perp) / dot(perp, perp);
    l = dot(view_position + t * view_normal - plume_position, plume_normal);
    r = dot(normalize(perp), view_position + t * view_normal - plume_position - l * plume_normal);
    //r = distance(view_position + t * view_normal, plume_position + l * plume_normal);

    return t > 0 && l > 0 && l < plume_length;
}

void main()
{
    FragColor = texture(fbo_texture, texCoord);
    vec3 color = FragColor.rgb;

    vec3 view_normal = normalize(far.xyz / far.w - near.xyz / near.w);

    float t, y, r;
    if (plume_intersect(view_normal, t, y, r)) {
        float R = plume_radius(y) * (plume_radiusScalar(y) * plume_roughness + 1);
        float x = R*R - r*r;

        if (x > 0) {
            float noise = texture(perlin_texture, vec2(r, y) * perlin_sample.z - perlin_sample.xy).x;
            noise /= plume_noise;

            noise -= .6 * r / R * r / cc_radius;
            noise -= y / plume_length;

            x = sqrt(x) / length(cross(view_normal, plume_normal));
            float light = 4 + 10 * exp(-(3 + 2 * noise) * (plume_radiusScalar(y) - x));

            color += clamp(noise, 0, 1) * x * light * plume_color;
            FragColor.a += noise > 0 ? 1 : 0;
        }
    }

    color = 1 - exp(-color * exposure);
    FragColor.rgb = pow(color, vec3(1 / gamma));
}
