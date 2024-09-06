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

    glm::vec3 barycenter(0);

    for (Vertex& v : vertices)
        v.value.x = 0;

    for (spring_t s : springs) {
        if (s.i == s.j) {
            //std::cout << "bad spring!" << std::endl;
            continue;
        }

        glm::vec3 w = vertices[s.i].position - vertices[s.j].position;
        glm::vec3 n = vertices[s.i].normal - vertices[s.j].normal;

        float d = s.length / glm::length(w);
        float a = (1 - d) / 2;
        float dA = std::min(5 * dt * a, a);

        vertices[s.i].position -= dA * w;
        vertices[s.j].position += dA * w;

        float curvature = s.length * glm::dot(n, w) / glm::dot(w, w);
        vertices[s.i].value.x += curvature;
        vertices[s.j].value.x += curvature;
    }

    for (spring_t s : edgeSprings) {
        //vertices[s.i].position += 0.01f * dt / s.length * (vertices[s.i].position - vertices[s.j].position);
    }

    for (int i = 0; i < vertices.size(); i++) {
        //vertices[i].position -= 0.001f * dt * vertices[i].value.x * vertices[i].normal;
        //vertices[i].position += 0.04f * dt * vertices[i].normal;
        barycenter += vertices[i].position;
    }

    barycenter /= vertices.size();

    // update normals
    std::vector<int> normals_count(vertices.size());
    std::vector<glm::vec3> normals_v(vertices.size());

    for (int i = 0; i < area.size(); i++) {
        glm::vec3& p0 = vertices[triangles[3 * i]].position;
        glm::vec3& p1 = vertices[triangles[3 * i + 1]].position;
        glm::vec3& p2 = vertices[triangles[3 * i + 2]].position;

        glm::vec3 w = glm::cross(p1 - p0, p2 - p0);
        float A0 = glm::length(w) / 2.f;
        glm::vec3 n = 2.f / A0 * w;

        normals_v[triangles[3 * i]] += n;
        normals_v[triangles[3 * i + 1]] += n;
        normals_v[triangles[3 * i + 2]] += n;
        normals_count[triangles[3 * i]]++;
        normals_count[triangles[3 * i + 1]]++;
        normals_count[triangles[3 * i + 2]]++;

        glm::vec3 bc = (p0 + p1 + p2) / 3.f;
        float dA = area[i] / A0;
        float dS = glm::sqrt(dA) - 1;

        p0 += 0.2f * dt * dS * (p0 - bc);
        p1 += 0.2f * dt * dS * (p1 - bc);
        p2 += 0.2f * dt * dS * (p2 - bc);

        //p0 += 0.4f * (1.f + 1.f * glm::exp(-stress_v[triangles[3 * i]])) * dt * w;
        //p1 += 0.4f * (1.f + 1.f * glm::exp(-stress_v[triangles[3 * i + 1]])) * dt * w;
        //p2 += 0.4f * (1.f + 1.f * glm::exp(-stress_v[triangles[3 * i + 2]])) * dt * w;
        p0 += 0.001f * par.pressure * dt * n;
        p1 += 0.001f * par.pressure * dt * n;
        p2 += 0.001f * par.pressure * dt * n;

        //vertices[triangles[i]].position += 0.4f * dt * glm::normalize(w);
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = normals_v[i] / (float)normals_count[i];
        glm::vec3 v = vertices[i].position - barycenter;
        float t = glm::dot(v, v);
        vertices[i].position = v * (1.f + par.explode * dt / t);
    }
}

//void Mesh::_iterate(float dt) {
//    for (int i = 0; i < identify.size(); i++) {
//        int j = identify[i];
//        if (i == j)
//            continue;
//
//        vertices[i].position = moveTowards(vertices[j].position, vertices[i].position + glm::vec3(0, 0, -dt / 2), dt);
//    }
//
//    //for (int i = 0; i < area.size(); i++) {
//    //    glm::vec3 p0 = vertices[triangles[3 * i]].position;
//    //    glm::vec3 p1 = vertices[triangles[3 * i + 1]].position;
//    //    glm::vec3 p2 = vertices[triangles[3 * i + 2]].position;
//    //             
//    //    glm::vec3 w = glm::cross(p1 - p0, p2 - p0);
//    //    float S = area[i] / glm::length(w);
//    //    std::cout << area[i] << "; " << glm::length(w) << std::endl;
//    //
//    //    vertices[triangles[3 * i + 1]].position = p0 + S * (p1 - p0);
//    //    vertices[triangles[3 * i + 2]].position = p0 + S * (p2 - p0);
//    //    //vertices[triangles[i]].position += 0.5f * dt * glm::normalize(w);
//    //}
//
//    std::vector<float> stress_v(vertices.size());
//
//    for (Vertex& v : vertices)
//        v.value.x = 0;
//
//    for (spring_t s : springs) {
//        glm::vec3 w = vertices[s.i].position - vertices[s.j].position;
//        glm::vec3 n = vertices[s.i].normal - vertices[s.j].normal;
//
//        float d = s.length / glm::length(w);
//        float a = (1 - d) / 2;
//
//        if (s.i % 20)
//            vertices[s.i].position -= a * w;
//
//        if (s.j % 20)
//            vertices[s.j].position += a * w;
//
//        float curvature = glm::dot(n, w) / glm::dot(w, w);
//        vertices[s.i].value.x += curvature;
//        vertices[s.j].value.x += curvature;
//
//        stress_v[s.i] += a;
//        stress_v[s.j] += a;
//    }
//
//    // update normals
//    std::vector<int> normals_count(vertices.size());
//    std::vector<glm::vec3> normals_v(vertices.size());
//
//    for (int i = 0; i < area.size(); i++) {
//        glm::vec3 p0 = vertices[triangles[3 * i]].position;
//        glm::vec3 p1 = vertices[triangles[3 * i + 1]].position;
//        glm::vec3 p2 = vertices[triangles[3 * i + 2]].position;
//
//        glm::vec3 w = glm::cross(p1 - p0, p2 - p0);
//        glm::vec3 n = glm::normalize(w);
//
//        normals_v[triangles[3 * i]] += n;
//        normals_v[triangles[3 * i + 1]] += n;
//        normals_v[triangles[3 * i + 2]] += n;
//        normals_count[triangles[3 * i]]++;
//        normals_count[triangles[3 * i + 1]]++;
//        normals_count[triangles[3 * i + 2]]++;
//
//        //vertices[triangles[i]].position += 0.4f * dt * glm::normalize(w);
//    }
//
//    for (int i = 0; i < vertices.size(); i++) {
//        if (identify[i] == i && i > 20 && i % 20 && i < 380)
//            vertices[i].position += 0.002f * glm::exp(-stress_v[i]) * vertices[i].normal;
//        
//        vertices[i].normal = normals_v[i] / (float)normals_count[i];
//
//        float t = glm::clamp(vertices[i].value.x / 5.f, .3f, 1.f) - .3f;
//        float s = .4f * glm::exp(1.f * t) * (1.f - t);
//        //if (identify[i] == i && i % 20)
//        //    vertices[i].position += dt * s * vertices[i].normal;
//    }
//}

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