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

void Mesh::init_rectangle(GLuint res_x, GLuint res_y) {
    vertices.clear();
    vertices.reserve(res_x * res_y);

    identify.clear();
    identify.reserve(res_x * res_y);

    for (float y = 0; y < res_y; y++) {
        for (float x = 0; x < res_x; x++) {
            glm::vec2 uv(x / (res_x - 1), y / (res_y - 1));

            identify.push_back(y * res_x + x);
            vertices.push_back(Vertex{
                glm::vec3(2.f * uv - 1.f, 0),
                glm::vec3(0, 0, 1),
                uv,
                glm::vec2(0)
            });
        }
    }

    triangles.clear();
    triangles.reserve(6 * (res_x - 1) * (res_y - 1));

    springs.clear();
    springs.reserve(2 * (res_x * res_y - res_x - res_y));

    area.clear();
    area.reserve(2 * (res_x - 1) * (res_y - 1));

    float d1 = 2.f / (res_x - 1);
    float d2 = 2.f / (res_y - 1);
    float d3 = glm::sqrt(d1*d1 + d2*d2);
    for (GLuint y = 0; y < res_y - 1; y++) {
        for (GLuint x = 0; x < res_x - 1; x++) {
            GLuint i = y * res_x + x;

            triangles.push_back(i);
            triangles.push_back(i + 1);
            triangles.push_back(i + res_x + 1);
            area.push_back(d1 * d2 / 2);

            triangles.push_back(i);
            triangles.push_back(i + res_x + 1);
            triangles.push_back(i + res_x);
            area.push_back(d1 * d2 / 2);

            springs.push_back(spring_t{ i + res_x, i + res_x + 1, d1 });
            springs.push_back(spring_t{ i + 1, i + res_x + 1, d2 });
        }
    }

    for (GLuint y = 0; y < res_y - 1; y++)
        springs.push_back(spring_t{ y * res_x, (y + 1) * res_x, d2 });
    
    for (GLuint x = 0; x < res_x - 1; x++)
        springs.push_back(spring_t{ x, x + 1, d1 });
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

    case MESH_PM:
        init_rectangle(res_x, res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

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

void debug_v(glm::vec3 w) {
    std::cout << w.x << ", " << w.y << ", " << w.z << std::endl;
}

void Mesh::iterate(float dt) {
    for (int i = 0; i < identify.size(); i++) {
        int j = identify[i];
        if (i == j)
            continue;

        vertices[i].position = moveTowards(vertices[j].position, vertices[i].position + glm::vec3(0, 0, -dt / 2), dt);
    }

    //for (int i = 0; i < area.size(); i++) {
    //    glm::vec3 p0 = vertices[triangles[3 * i]].position;
    //    glm::vec3 p1 = vertices[triangles[3 * i + 1]].position;
    //    glm::vec3 p2 = vertices[triangles[3 * i + 2]].position;
    //             
    //    glm::vec3 w = glm::cross(p1 - p0, p2 - p0);
    //    float S = area[i] / glm::length(w);
    //    std::cout << area[i] << "; " << glm::length(w) << std::endl;
    //
    //    vertices[triangles[3 * i + 1]].position = p0 + S * (p1 - p0);
    //    vertices[triangles[3 * i + 2]].position = p0 + S * (p2 - p0);
    //    //vertices[triangles[i]].position += 0.5f * dt * glm::normalize(w);
    //}

    for (Vertex& v : vertices)
        v.value.x = 0;

    for (spring_t s : springs) {
        glm::vec3 w = vertices[s.i].position - vertices[s.j].position;
        glm::vec3 n = vertices[s.i].normal - vertices[s.j].normal;

        float d = s.length / glm::length(w);
        float a = (1 - d) / 2;

        vertices[s.i].position -= a * w;
        vertices[s.j].position += a * w;

        float curvature = glm::dot(n, w) / glm::dot(w, w);
        vertices[s.i].value.x += curvature;
        vertices[s.j].value.x += curvature;
    }

    // update normals
    std::vector<float> normals_count(vertices.size());
    std::vector<glm::vec3> normals_v(vertices.size());

    for (int i = 0; i < area.size(); i++) {
        glm::vec3 p0 = vertices[triangles[3 * i]].position;
        glm::vec3 p1 = vertices[triangles[3 * i + 1]].position;
        glm::vec3 p2 = vertices[triangles[3 * i + 2]].position;

        glm::vec3 w = glm::cross(p1 - p0, p2 - p0);
        glm::vec3 n = glm::normalize(w);

        normals_v[triangles[3 * i]] += n;
        normals_v[triangles[3 * i + 1]] += n;
        normals_v[triangles[3 * i + 2]] += n;
        normals_count[triangles[3 * i]]++;
        normals_count[triangles[3 * i + 1]]++;
        normals_count[triangles[3 * i + 2]]++;
        //vertices[triangles[i]].position += 0.4f * dt * glm::normalize(w);
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = normals_v[i] / normals_count[i];

        if (identify[i] == i)
            vertices[i].position += dt * 0.01f * glm::abs(vertices[i].value.x) * vertices[i].normal;
    }
}

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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