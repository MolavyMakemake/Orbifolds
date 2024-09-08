#include "Mesh.h"

void Mesh::computeMesh(GLuint res_x, GLuint res_y, SHAPE_ shape,
    std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {

    switch (shape)
    {
    case SHAPE_2222:
        init2222(res_x, res_y, identify, _pos);
        break;
    case SHAPE_333:
        init333(res_x, identify, _pos);
        break;
    case SHAPE_442:
        init442(res_x, identify, _pos);
        break;
    case SHAPE_662:
        init662(res_x, identify, _pos);
        break;
    case SHAPE_632:
        init632(res_x, identify, _pos);
        break;
    default:
        break;
    }
}

void Mesh::_triangulate(std::vector<GLuint>& identify, GLuint i, GLuint j, GLuint k) {
    if (identify[i] == identify[j] || identify[i] == identify[k] || identify[j] == identify[k])
        return;

    glm::vec3 p1 = vertices[j].position - vertices[i].position;
    glm::vec3 p2 = vertices[k].position - vertices[i].position;
    glm::vec3 v = glm::normalize(p1 + p2);

    triangles.push_back(identify[i]);
    triangles.push_back(identify[j]);
    triangles.push_back(identify[k]);
    shapes.push_back(shape_t{
        glm::dot(v, p1),
        glm::dot(v, p2),
        glm::length(p1 - v * glm::dot(v, p1))
        });
}

void Mesh::init2222(GLuint res_x, GLuint res_y,
    std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float))
{
    vertices.clear();
    vertices.reserve(res_x * res_y);

    float dx = 1.f / (res_x - 1);
    for (float y = 0; y < res_y; y++) {
        for (float x = 0; x < res_x; x++) {
            glm::vec2 uv(x / (res_x - 1), y / (res_y - 1));

            vertices.push_back(Vertex{
                _pos(uv.x, uv.y),
                glm::vec3(0, 0, 1),
                uv,
                glm::vec2(0)
                });
        }
    }

    triangles.clear();
    triangles.reserve(6 * (res_x - 1) * (res_y - 1));

    shapes.clear();
    shapes.reserve(2 * (res_x - 1) * (res_y - 1));

    int i = vertices.size();
    for (GLuint y = 0; y < res_y - 1; y++) {
        for (GLuint x = 0; x < res_x - 1; x++, i++) {
            GLuint i0 = y * res_x + x, i1 = i0 + 1, i2 = i0 + res_x, i3 = i1 + res_x;
            glm::vec2 uv = .25f * (vertices[i0].texCoords + vertices[i1].texCoords + vertices[i2].texCoords + vertices[i3].texCoords);

            vertices.push_back(Vertex{
                _pos(uv.x, uv.y),
                glm::vec3(0, 0, 1),
                uv,
                glm::vec2(0)
                });
            identify.push_back(i);

            _triangulate(identify, i0, i1, i);
            _triangulate(identify, i1, i3, i);
            _triangulate(identify, i2, i0, i);
            _triangulate(identify, i3, i2, i);
        }
    }
}

void Mesh::init442(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float))
{
    vertices.clear();
    vertices.reserve((res + 1) * res / 2);

    int i = 0;
    for (float y = 0; y < res; y++) {
        for (float x = 0; x < res - y; x++) {
            glm::vec2 uv(x / (res - 1), y / (res - 1));

            vertices.push_back(Vertex{
                _pos(uv.x, uv.y),
                glm::vec3(0, 0, 1),
                uv,
                glm::vec2(0)
                });
        }
    }

    triangles.clear();
    shapes.clear();

    int i0 = 0;
    for (GLuint y = 0; y < res - 1; y++, i0++) {
        for (GLuint x = 0; x < res - y - 1; x++, i0++) {
            GLuint i1 = i0 + 1, i2 = i0 + res - y, i3 = i2 + 1;

            _triangulate(identify, i0, i1, i2);

            if (x + 2 < res - y)
                _triangulate(identify, i1, i3, i2);
        }
    }
}

void Mesh::init333(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {
    auto pos = [](float x, float y) {
        return glm::vec3(2.f * x - 1 + y, phi * (y - 1 / 3.f), 0);
    };
    init442(res, identify, pos);
}

void Mesh::init632(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {
    auto pos = [](float x, float y) {
        return glm::vec3(2.f * x - .5f, phi / 3.f * (y - 1 / 3.f), 0);
    };
    init442(res, identify, pos);
}

void Mesh::init662(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {
    auto pos = [](float x, float y) {
        return glm::vec3(x - .5f * y, .75f * y, 0);
    };
    init442(res, identify, pos);
}