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

void Mesh::init_rectangle(GLuint res_x, GLuint res_y, std::vector<GLuint>& identify, std::vector<GLuint>& edge) {
    GLuint c = 0;
    std::vector<GLuint> vertex_i(res_x * res_y);

    for (int i = 0; i < identify.size(); i++) {
        bool isNode = identify[i] == i;
        vertex_i[i] = isNode * (identify[i] - c);
        c += !isNode;
    }
    for (int i = 0; i < identify.size(); i++)
        vertex_i[i] = vertex_i[identify[i]];

    vertices.clear();
    vertices.reserve(res_x * res_y);

    for (float y = 0; y < res_y; y++) {
        for (float x = 0; x < res_x; x++) {
            if (identify[y * res_x + x] != y * res_x + x)
                continue;

            glm::vec2 uv(x / (res_x - 1), y / (res_y - 1));

            vertices.push_back(Vertex{
                glm::vec3(2 * x / (res_x - 1) - 1, 2 * y / (res_y - 1) - 1, 0),
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
            GLuint _i = y * res_x + x;

            GLuint i = vertex_i[_i], j = vertex_i[_i + 1],
                k = vertex_i[_i + res_x], l = vertex_i[_i + res_x + 1];

            triangles.push_back(i);
            triangles.push_back(j);
            triangles.push_back(l);
            area.push_back(d1 * d2 / 2);

            triangles.push_back(i);
            triangles.push_back(l);
            triangles.push_back(k);
            area.push_back(d1 * d2 / 2);

            springs.push_back(spring_t{ k, l, d1 });
            springs.push_back(spring_t{ j, l, d2 });
            springs.push_back(spring_t{ i, l, d3 });
        }
    }

    for (GLuint y = 0; y < res_y - 1; y++)
        springs.push_back(spring_t{ vertex_i[y * res_x], vertex_i[(y + 1) * res_x], d2});
    
    for (GLuint x = 0; x < res_x - 1; x++)
        springs.push_back(spring_t{ vertex_i[x], vertex_i[x + 1], d1 });

    edgeSprings.clear();
    for (GLuint i : edge) {
        unsigned char flag;
        flag = i < res_x;
        flag |= (i >= res_x * (res_y - 1)) << 1;
        flag |= !(i % res_x) << 2;
        flag |= !((i - 1) % res_x) << 3;

        switch (flag)
        {
        case 1: // bottom
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res_x], d2 });
            break;
        case 2: // top
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - res_x], d2 });
            break;
        case 4: // left
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + 1], d2 });
            break;
        case 8: // right
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - 1], d2 });
            break;

        case 5:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res_x + 1], d3 });
            break;
        case 6:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - res_x + 1], d3 });
            break;
        case 9:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res_x - 1], d3 });
            break;
        case 10:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - res_x - 1], d3 });
            break;

        default:
            std::cout << "bad edge element: " << i << ". flag: " << (int)flag << std::endl;
        }
    }
}

const char hex_arr[] = "0123456789abcdef";
std::string hex(int i) {
    return std::string({ 
        hex_arr[(i & 0x00F0) >> 4], 
        hex_arr[(i & 0x000F) >> 0] });
}

std::vector<GLuint> arange(int N) {
    std::vector<GLuint> r;
    r.reserve(N);
    for (int i = 0; i < N; i++)
        r.push_back(i);

    return r;
}

void Mesh::Reset() {
    Delete();
    computeDomain();
    Generate();
}

void Mesh::computeDomain() {
    std::vector<GLuint> identify;
    std::vector<GLuint> edge;

    iteration = 0;

    switch (domain)
    {
    case MESH_RECTANGLE:
        identify = arange(res_x * res_y);
        init_rectangle(res_x, res_y, identify, edge);
        break;

    case MESH_P1:
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 0; y < res_y - 1; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

        identify[res_x * res_y - 1] = 0;

        init_rectangle(res_x, res_y, identify, edge);
        break;

    case MESH_PM:
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

        for (GLuint x = 0; x < res_x - 1; x++) {
            edge.push_back(x);
            edge.push_back(res_x * (res_x - 1) + x);
        }

        init_rectangle(res_x, res_y, identify, edge);
        break;

    case MESH_CM:
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x * res_y - 1 - y * res_x] = y * res_x;

        for (float y = 0; y < res_y; y++) {
            for (float x = 0; x < res_x; x++) {
                std::cout << hex(identify[y * res_x + x]) << " ";
            }
            std::cout << std::endl;
        }

        for (GLuint x = 0; x < res_x - 1; x++) {
            edge.push_back(x);
            edge.push_back(res_x * (res_x - 1) + x);
        }

        init_rectangle(res_x, res_y, identify, edge);
        break;

    case MESH_P4:
        identify = arange(res_x * res_x);

        for (GLuint i = 1; i < res_x - 1; i++) {
            identify[res_x * i] = i;
            identify[res_x * (res_x - 1) + i] = (i + 1) * res_x - 1;
        }

        identify[res_x * (res_x - 1)] = res_x - 1;

        for (float y = 0; y < res_y; y++) {
            for (float x = 0; x < res_x; x++) {
                std::cout << hex(identify[y * res_x + x]) << " ";
            }
            std::cout << std::endl;
        }

        init_rectangle(res_x, res_x, identify, edge);
        break;


    default:
        break;
    }
}

Mesh::Mesh(MESH_ domain, GLuint res_x, GLuint res_y) : domain(domain), res_x(res_x), res_y(res_y) {
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

    std::vector<float> stress_v(vertices.size());
    glm::vec3 barycenter(0);

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

        stress_v[s.i] += std::max<float>(a, 0);
        stress_v[s.j] += std::max<float>(a, 0);
    }

    for (spring_t s : edgeSprings) {
        vertices[s.i].position += 0.1f * dt * glm::exp(-stress_v[s.i]) / s.length * (vertices[s.i].position - vertices[s.j].position);
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position += 0.01f * dt * glm::exp(-stress_v[i]) * vertices[i].normal;
        //vertices[i].position += 0.001f * vertices[i].normal;
        barycenter += vertices[i].position;
    }

    barycenter /= vertices.size();

    // update normals
    std::vector<int> normals_count(vertices.size());
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
        vertices[i].normal = normals_v[i] / (float)normals_count[i];
        vertices[i].position -= barycenter;
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