#include"Mesh.h"
#include <glm/geometric.hpp>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& triangles,
    const std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->triangles = triangles;

    Generate();
}

void Mesh::Generate() {
    // Generate buffers and vertex array
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Link vertex array to the buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint),
        &triangles[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    //// vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);
}

void Mesh::Update() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
}

void Mesh::Draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Delete() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::init_rectangle(GLuint res_x, GLuint res_y) {
    vertices.clear();
    vertices.reserve(res_x * res_y);

    identify.clear();
    identify.reserve(res_x * res_y);

    for (float y = 0; y < res_y; y++) {
        for (float x = 0; x < res_x; x++) {
            identify.push_back(y * res_x + x);
            vertices.push_back(Vertex{ 
                glm::vec3(2 * x / (res_x - 1) - 1, 2 * y / (res_y - 1) - 1, 0),
                glm::vec2(x / (res_x - 1), y / (res_y - 1))
            });
        }
    }

    triangles.clear();
    triangles.reserve(6 * (res_x - 1) * (res_y - 1));

    for (int y = 0; y < res_y - 1; y++) {
        for (int x = 0; x < res_x - 1; x++) {
            int i = y * res_x + x;
            triangles.push_back(i);
            triangles.push_back(i + res_x + 1);
            triangles.push_back(i + res_x);
            triangles.push_back(i);
            triangles.push_back(i + 1);
            triangles.push_back(i + res_x + 1);
        }
    }
}

const char hex_arr[] = "0123456789abcdef";
std::string hex(int i) {
    return std::string({ 
        hex_arr[(i & 0x00F0) >> 4], 
        hex_arr[(i & 0x000F) >> 0] });
}

Mesh::Mesh(MESH_ domain, GLuint res_x, GLuint res_y) {
    switch (domain)
    {
    case MESH_RECTANGLE:
        init_rectangle(res_x, res_y);
        break;

    case MESH_P1:
        init_rectangle(res_x, res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 0; y < res_y - 1; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

        identify[res_x * res_y - 1] = 0;

        break;

    default:
        break;
    }

    for (float y = 0; y < res_y; y++) {
        for (float x = 0; x < res_x; x++) {
            std::cout << hex(identify[y * res_x + x]) << " ";
        }
        std::cout << std::endl;
    }

    Generate();
}

glm::vec3 moveTowards(glm::vec3 to, glm::vec3 from, float step) {
    glm::vec3 dpos = to - from;
    float distance = glm::length(dpos);

    return from + dpos * fmin(step / distance, 1.f);
}

void Mesh::iterate(float dt) {
    //for (int i = 0; i < triangles.size(); i += 3) {
    //
    //}

    for (int i = 0; i < identify.size(); i++) {
        int j = identify[i];
        if (i == j)
            continue;

        vertices[i].position = moveTowards(vertices[j].position, vertices[i].position + glm::vec3(0, 0, dt / 2), dt);
    }
}