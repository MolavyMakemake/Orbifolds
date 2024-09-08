#include"Mesh.h"
#include <glm/geometric.hpp>
#include <stb/stb_image.h>

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
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex value
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, value));

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

GLuint resolve(std::vector<GLuint>* identify, std::vector<GLuint>* trace) {
    return 0;
}

void Mesh::Reset() {
    Delete();
    computeDomain();
    Generate();
}

Mesh::Mesh(MESH_ domain, GLuint res_x, GLuint res_y) 
    : domain(domain), resolution(res_x, res_y) {
    computeDomain();
    Generate();
}

glm::vec3 moveTowards(glm::vec3 to, glm::vec3 from, float step) {
    glm::vec3 dpos = to - from;
    float distance = glm::length(dpos);

    return from + dpos * fmin(step / distance, 1.f);
}

void debug_v(glm::vec3 w) {
    std::cout << w.x << ", " << w.y << ", " << w.z << std::endl;
}

void Mesh::iterate(float dt) {
    iteration++;

    //for (Vertex& v : vertices)
    //    v.position += par.pressure * 0.1f * dt * v.normal;

    std::vector<glm::vec3> forces_v(vertices.size());
    std::vector<glm::vec3> normals_v(vertices.size());
    std::vector<int> normals_count(vertices.size());

    for (int T = 0; T < shapes.size(); T++) {
        glm::vec3 p0 = vertices[triangles[3 * T + 0]].position;
        glm::vec3 p1 = vertices[triangles[3 * T + 1]].position;
        glm::vec3 p2 = vertices[triangles[3 * T + 2]].position;

        glm::vec3 v1 = p1 - p0;
        glm::vec3 v2 = p2 - p0;

        glm::vec3 x = glm::normalize(v1 + v2);
        glm::vec3 y = glm::normalize(v1 - x * glm::dot(v1, x));
        glm::vec3 z = glm::cross(y, x);

        if (isnan(y.x))
            continue;

        // fix normals
        normals_v[triangles[3 * T + 0]] += z;
        normals_v[triangles[3 * T + 1]] += z;
        normals_v[triangles[3 * T + 2]] += z;
        normals_count[triangles[3 * T + 0]]++;
        normals_count[triangles[3 * T + 1]]++;
        normals_count[triangles[3 * T + 2]]++;

        // fix triangles
        glm::vec3 q1 = shapes[T].l1 * x + shapes[T].w * y - v1;
        glm::vec3 q2 = shapes[T].l2 * x - shapes[T].w * y - v2;
        glm::vec3 d = -(q1 + q2) / 3.f;

        // apply forces
        glm::vec3 a = (shapes[T].l1 + shapes[T].l2) * shapes[T].w * z;
        forces_v[triangles[3 * T + 0]] += par.k * d + par.pressure * a;
        forces_v[triangles[3 * T + 1]] += par.k * (q1 + d) + par.pressure * a;
        forces_v[triangles[3 * T + 2]] += par.k * (q2 + d) + par.pressure * a;
    }

    std::vector<glm::vec3> position_v(vertices.size());
    std::vector<int> position_count(vertices.size());

    glm::vec3 barycenter(0);

    for (int T = 0; T < shapes.size(); T++) {
        glm::vec3 _bc = vertices[triangles[3 * T + 0]].position
            + vertices[triangles[3 * T + 1]].position
            + vertices[triangles[3 * T + 2]].position;

        barycenter += _bc;
        position_v[triangles[3 * T + 0]] += _bc;
        position_v[triangles[3 * T + 1]] += _bc;
        position_v[triangles[3 * T + 2]] += _bc;
        position_count[triangles[3 * T + 0]] += 3;
        position_count[triangles[3 * T + 1]] += 3;
        position_count[triangles[3 * T + 2]] += 3;
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = normals_v[i] / (float)normals_count[i];
        forces_v[i] += par.smoothing * glm::dot(position_v[i] / (float)position_count[i] - vertices[i].position, vertices[i].normal) * vertices[i].normal;
    }

    for (GLuint e : edge) {
        glm::vec3 n = vertices[e].position - position_v[e] / (float)position_count[e];
        forces_v[e] += par.stretch * n;
    }

    barycenter /= 3.f * (float)shapes.size();

    for (int i = 0; i < vertices.size(); i++) {
        forces_v[i] -= par.dampening * velocity[i];
        glm::vec3 v = vertices[i].position - barycenter;
        float t = glm::max(glm::dot(v, v), .0001f);
        forces_v[i] += par.explode / t * v;

        velocity[i] = glm::clamp(velocity[i] + dt * forces_v[i], -.1f, .1f);
        vertices[i].position += dt * velocity[i] - barycenter;
    }
}

/*
void Mesh::iterate(float dt) {
    iteration++;

    //for (Vertex& v : vertices)
    //    v.position += par.pressure * 0.1f * dt * v.normal;

    std::vector<glm::vec3> normals_v(vertices.size());
    std::vector<int> normals_count(vertices.size());

    for (int T = 0; T < shapes.size(); T++) {
        glm::vec3& p0 = vertices[triangles[3 * T + 0]].position;
        glm::vec3& p1 = vertices[triangles[3 * T + 1]].position;
        glm::vec3& p2 = vertices[triangles[3 * T + 2]].position;

        glm::vec3 v1 = p1 - p0;
        glm::vec3 v2 = p2 - p0;

        glm::vec3 x = glm::normalize(v1 + v2);
        glm::vec3 y = glm::normalize(v1 - x * glm::dot(v1, x));
        glm::vec3 z = glm::cross(y, x);

        if (isnan(y.x))
            continue;

        // fix normals
        normals_v[triangles[3 * T + 0]] += z;
        normals_v[triangles[3 * T + 1]] += z;
        normals_v[triangles[3 * T + 2]] += z;
        normals_count[triangles[3 * T + 0]]++;
        normals_count[triangles[3 * T + 1]]++;
        normals_count[triangles[3 * T + 2]]++;

        // fix triangles
        glm::vec3 q1 = shapes[T].l1 * x + shapes[T].w * y - v1;
        glm::vec3 q2 = shapes[T].l2 * x - shapes[T].w * y - v2;
        glm::vec3 d = -(q1 + q2) / 3.f;

        p0 += dt * d;
        p1 += dt * (q1 + d);
        p2 += dt * (q2 + d);

        //std::cout << T << ": " << glm::distance(q1, p1) << ", ";
        //std::cout << glm::distance(q2, p2) << std::endl;
    }

    std::vector<glm::vec3> position_v(vertices.size());
    std::vector<int> position_count(vertices.size());

    glm::vec3 barycenter(0);

    for (int T = 0; T < shapes.size(); T++) {
        glm::vec3 _bc = vertices[triangles[3 * T + 0]].position
                      + vertices[triangles[3 * T + 1]].position 
                      + vertices[triangles[3 * T + 2]].position;

        barycenter += _bc;
        position_v[triangles[3 * T + 0]] += _bc;
        position_v[triangles[3 * T + 1]] += _bc;
        position_v[triangles[3 * T + 2]] += _bc;
        position_count[triangles[3 * T + 0]] += 3;
        position_count[triangles[3 * T + 1]] += 3;
        position_count[triangles[3 * T + 2]] += 3;
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = normals_v[i] / (float)normals_count[i];
    }

    for (GLuint e : edge) {
        glm::vec3& p = vertices[e].position;
        vertices[e].normal = glm::normalize(p - position_v[e] / (float)position_count[e]);
    }

    barycenter /= 3.f * (float)shapes.size();

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position += dt * .1f * par.pressure * vertices[i].normal;

        glm::vec3 v = vertices[i].position - barycenter;
        float t = glm::max(glm::dot(v, v), .0001f);
        vertices[i].position = v * (1.f + 2.f * par.explode * dt / t);
    }
}*/

GLuint textureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internal_format{}, format{};

        if (nrComponents == 1) {
            format = GL_RED;
            internal_format = GL_RED;
        }
        else if (nrComponents == 3) {
            internal_format = GL_SRGB;
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            internal_format = GL_SRGB_ALPHA;
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << directory << "/" << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}