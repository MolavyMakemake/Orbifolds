#include "Skybox.h"
#include <glm/gtc/type_ptr.hpp>

void Skybox::Draw(Shader &shader, Camera& camera) {
    glDepthMask(GL_FALSE);
    shader.Activate();

    glm::mat4 view = glm::mat4(glm::mat3(camera.view));
    glUniformMatrix4fv(shader.Loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.Loc("proj"), 1, GL_FALSE, glm::value_ptr(camera.proj));

    glBindVertexArray(VAO);
    cubemap.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

Skybox::Skybox(Cubemap cubemap) : cubemap(cubemap) {
    // Generate buffers and vertex array
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Link vertex array to the buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

