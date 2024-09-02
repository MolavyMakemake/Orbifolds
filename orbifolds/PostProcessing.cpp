#include "PostProccessing.h"
#include <glm/gtc/type_ptr.hpp>

PostProcessing::PostProcessing() {
    // Generate buffers and vertex array
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Link vertex array to the buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

void PostProcessing::Draw(Shader& shader) {
    glUniform1i(shader.Loc("fbo_texture"), 0);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void PostProcessing::Draw(Shader& shader, Scene& scene) {
    glm::mat4 view = glm::mat4(glm::mat3(scene.camera.view));
    glUniformMatrix4fv(shader.Loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.Loc("proj"), 1, GL_FALSE, glm::value_ptr(scene.camera.proj));

    glUniform3fv(shader.Loc("view_position"), 1, (float*)&scene.camera.position);
    glUniform1f(shader.Loc("gamma"), scene.camera.gamma);
    glUniform1f(shader.Loc("exposure"), scene.camera.exposure);
    glUniform1f(shader.Loc("near"), scene.camera.nearClippingPlane);
    glUniform1f(shader.Loc("far"), scene.camera.farClippingPlane);


    glUniform1i(shader.Loc("fbo_texture"), 0);
    glUniform1i(shader.Loc("depth_texture"), 1);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

float interpLength(float x, float t) {
    if (x > 5)
        return 1.f;

    if (x > 3)
        return .5f;

    return .1f;
}
float interpVelocity(float x, float t) {
    if (x > 5)
        return 30.f;

    if (x > 3)
        return 15.f;

    return 4.5f;
}

float length[] = { 2, 20, 20 };
float slope[] = { 3, 1, -.01, };

float interpRoughness(float x) {
    if (x > 5)
        return .1f;

    return 0;
}
int interpState(float x) {
    return (x > 3) + (x > 5);
}

float moveTowards(float x, float target, float step_pos, float step_neg) {
    if (x < target) {
        return std::min(x + step_pos, target);
    }
    else
        return std::max(x - step_neg, target);
}

float mem_t = 0;

void PostProcessing::Draw(Shader& shader, Scene& scene, plumeSettings_t& plumeSettings, plume_t& plume, unsigned int perlin) {
    float t = (std::clock() - plumeSettings.t_clk) / (float)CLOCKS_PER_SEC;
    float x = std::clamp<float>(plumeSettings.cc_pressure, 0, 20);
    
    int _state = interpState(x);
    if (plumeSettings.state != _state) {
        plumeSettings.t_clk = std::clock();
        plumeSettings.state = _state;
    }

    float dt = std::max(t - mem_t, 0.f);
    mem_t = t;

    glm::vec3 perlin_sample = glm::vec3(0.f, std::fmod(plume.velocity / 20.f * (std::clock()) / (float)CLOCKS_PER_SEC, 1.f), .1f);

    glUniform3fv(shader.Loc("plume_position"), 1, &plumeSettings.position[0]);
    glUniform3fv(shader.Loc("plume_normal"), 1, &plumeSettings.normal[0]);
    glUniform3fv(shader.Loc("plume_color"), 1, &plumeSettings.color[0]);
    glUniform1f(shader.Loc("plume_t"), t);
    glUniform1f(shader.Loc("plume_roughness"), plume.roughness);
    glUniform1f(shader.Loc("plume_length"), plume.length);
    glUniform1f(shader.Loc("plume_slope"), plume.slope);
    glUniform1f(shader.Loc("plume_noise"), plume.noise);
    glUniform1f(shader.Loc("plume_seperation"), plume.seperation);
    glUniform1f(shader.Loc("cc_pressure"), plumeSettings.cc_pressure);
    glUniform1f(shader.Loc("cc_radius"), plumeSettings.cc_radius);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, perlin);

    glUniform3fv(shader.Loc("perlin_sample"), 1, &perlin_sample[0]);
    glUniform1i(shader.Loc("perlin_texture"), 2);

    Draw(shader, scene);
}

void PostProcessing::Draw(Shader& shader, Scene &scene, Cubemap& cubemap) {
    glUniformMatrix4fv(shader.Loc("view"), 1, GL_FALSE, glm::value_ptr(scene.camera.view));
    glUniformMatrix4fv(shader.Loc("proj"), 1, GL_FALSE, glm::value_ptr(scene.camera.proj));

    glUniform3fv(shader.Loc("view_position"), 1, (float*)&scene.camera.position);
    glUniform1f(shader.Loc("gamma"), scene.camera.gamma);
    glUniform1f(shader.Loc("exposure"), scene.camera.exposure);
    //glUniform1f(shader.Loc("near"), scene.camera.nearClippingPlane);
    //glUniform1f(shader.Loc("far"), scene.camera.farClippingPlane);


    glUniform1i(shader.Loc("fbo_texture"), 0);
    glUniform1i(shader.Loc("depth_texture"), 1);
    glUniform1i(shader.Loc("starmap"), 2);

    glActiveTexture(GL_TEXTURE2);
    cubemap.Bind();

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}