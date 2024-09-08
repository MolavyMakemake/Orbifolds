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
    case SHAPE_632:
        init632(res_x, identify, _pos);
        break;
    default:
        break;
    }
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

    for (GLuint y = 0; y < res_y - 1; y++) {
        for (GLuint x = 0; x < res_x - 1; x++) {
            GLuint i0 = y * res_x + x, i1 = i0 + 1, i2 = i0 + res_x, i3 = i1 + res_x;

            glm::vec3 p1 = vertices[i1].position - vertices[i0].position;
            glm::vec3 p2 = vertices[i2].position - vertices[i0].position;
            glm::vec3 p3 = vertices[i3].position - vertices[i0].position;

            glm::vec3 v1 = glm::normalize(p1 + p3);
            glm::vec3 v2 = glm::normalize(p2 + p3);

            if (identify[i0] != identify[i1] && identify[i0] != identify[i3] && identify[i1] != identify[i3]) {
                triangles.push_back(identify[i0]);
                triangles.push_back(identify[i1]);
                triangles.push_back(identify[i3]);
                shapes.push_back(shape_t{
                    glm::dot(v1, p1),
                    glm::dot(v1, p3),
                    glm::length(p1 - v1 * glm::dot(v1, p1))
                    });
            }

            if (identify[i0] != identify[i3] && identify[i0] != identify[i2] && identify[i3] != identify[i2]) {
                triangles.push_back(identify[i0]);
                triangles.push_back(identify[i3]);
                triangles.push_back(identify[i2]);
                shapes.push_back(shape_t{
                    glm::dot(v2, p3),
                    glm::dot(v2, p2),
                    glm::length(p3 - v2 * glm::dot(v2, p3))
                    });
            }
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

            glm::vec3 p1 = vertices[i1].position - vertices[i0].position;
            glm::vec3 p2 = vertices[i2].position - vertices[i0].position;

            glm::vec3 v1 = glm::normalize(p1 + p2);

            if (identify[i0] != identify[i1] && identify[i0] != identify[i2] && identify[i1] != identify[i2]) {
                triangles.push_back(identify[i0]);
                triangles.push_back(identify[i1]);
                triangles.push_back(identify[i2]);
                shapes.push_back(shape_t{
                        glm::dot(v1, p1),
                        glm::dot(v1, p2),
                        glm::length(p1 - v1 * glm::dot(v1, p1))
                    });
            }

            if (x + 2 < res - y && identify[i1] != identify[i3] && identify[i1] != identify[i2] && identify[i3] != identify[i2]) {
                p2 -= p1;
                glm::vec3 p3 = vertices[i3].position - vertices[i1].position;
                glm::vec3 v2 = glm::normalize(p2 + p3);

                triangles.push_back(identify[i1]);
                triangles.push_back(identify[i3]);
                triangles.push_back(identify[i2]);

                shapes.push_back(shape_t{
                    glm::dot(v2, p3),
                    glm::dot(v2, p2),
                    glm::length(p3 - v2 * glm::dot(v2, p3))
                    });

                continue;
            }
        }
    }
}

void Mesh::init333(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {
    auto pos = [](float x, float y) {
        return glm::vec3(x, y, 0);
    };
    init442(res, identify, pos);
}

void Mesh::init632(GLuint res, std::vector<GLuint>& identify, glm::vec3(*_pos)(float, float)) {
    auto pos = [](float x, float y) {
        return glm::vec3(x, y, 0);
    };
    init442(res, identify, pos);
}