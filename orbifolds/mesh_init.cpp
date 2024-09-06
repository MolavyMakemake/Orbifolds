#include "Mesh.h"

void Mesh::computeMesh(GLuint res_x, GLuint res_y, SHAPE_ shape,
    std::vector<GLuint>& identify, std::vector<GLuint>& edge, float (*_len)(float)) {

    switch (shape)
    {
    case SHAPE_2222:
        init2222(res_x, res_y, identify, edge, _len);
        break;
    case SHAPE_333:
        init333(res_x, identify, edge);
        break;
    case SHAPE_442:
        init442(res_x, identify, edge);
        break;
    case SHAPE_632:
        init632(res_x, identify, edge);
        break;
    case SHAPE_RHOMBUS: 
    {
        init2222(res_x, res_x, identify, edge, _len);
        float phi = 1.118034f;
        for (Vertex& v : vertices) {
            v.position.x = phi * v.position.x + .5f * v.position.y;
        }

        for (float& a : area)
            a *= (phi + .5f);

        float psi = 1.345f;
        for (spring_t& s : springs) {
            bool d3 = (s.i / res_x) != (s.j / res_x) && (s.i % res_x) != (s.j % res_x);
            s.length = d3 ? psi * s.length : phi * s.length;
        }
        break;
    }
    default:
        break;
    }
}

void Mesh::init2222(GLuint res_x, GLuint res_y, 
    std::vector<GLuint>& identify, std::vector<GLuint>& edge,
    float (*_len)(float)) 
{
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
                glm::vec3(_len(uv.y) * (2 * x / (res_x - 1) - 1), 
                2 * y / (res_y - 1) - 1, 0),
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

    float dx = 2.f / (res_x - 1);
    float dy = 2.f / (res_y - 1);

    for (GLuint y = 0; y < res_y - 1; y++) {
        float s1 = _len(y / (res_y - 1.f));
        float s2 = _len((y + 1.f) / (res_y - 1.f));

        float d1 = glm::abs(s1 * dx);

        for (GLuint x = 0; x < res_x - 1; x++) {
            GLuint _i = y * res_x + x;
            float t = 2.f * x / (res_x - 1.f) - 1;
            float u = (t + dx) * (s2 - s1);
            float v = s2 * (t + dx) - s1 * t;
            float d2 = glm::sqrt(u * u + dy * dy);
            float d3 = glm::sqrt(v * v + dy * dy);

            GLuint i = vertex_i[_i], j = vertex_i[_i + 1],
                k = vertex_i[_i + res_x], l = vertex_i[_i + res_x + 1];

            triangles.push_back(i);
            triangles.push_back(j);
            triangles.push_back(l);
            area.push_back(d1 * dy / 2);

            triangles.push_back(i);
            triangles.push_back(l);
            triangles.push_back(k);
            area.push_back(d1 * dy / 2);

            springs.push_back(spring_t{ k, l, d1 });
            springs.push_back(spring_t{ j, l, d2 });
            springs.push_back(spring_t{ i, l, d3 });
        }
    }

    for (GLuint y = 0; y < res_y - 1; y++) {
        float t = y / (res_y - 1.f);
        float ds = _len(y + dy) - _len(y);
        springs.push_back(spring_t{ vertex_i[y * res_x], vertex_i[(y + 1) * res_x], 
            glm::sqrt(ds * ds + dy * dy) });
    }

    float d1_0 = _len(0) * dx;
    for (GLuint x = 0; x < res_x - 1; x++)
        springs.push_back(spring_t{ vertex_i[x], vertex_i[x + 1], d1_0 });

    // this part is not quite right
    edgeSprings.clear();
    float d3 = glm::sqrt(dx * dx + dy * dy);
    for (GLuint i : edge) {
        unsigned char flag;
        flag = i < res_x;
        flag |= (i >= res_x * (res_y - 1)) << 1;
        flag |= !(i % res_x) << 2;
        flag |= !((i - 1) % res_x) << 3;

        switch (flag)
        {
        case 1: // bottom
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res_x], dy });
            break;
        case 2: // top
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - res_x], dy });
            break;
        case 4: // left
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + 1], dx });
            break;
        case 8: // right
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - 1], dx });
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

void Mesh::init442(GLuint res, std::vector<GLuint>& identify, std::vector<GLuint>& edge, 
    float _shift, float _stretch) 
{
    GLuint c = 0;
    std::vector<GLuint> vertex_i((res + 1) * res / 2);

    for (int i = 0; i < identify.size(); i++) {
        bool isNode = identify[i] == i;
        vertex_i[i] = isNode * (identify[i] - c);
        c += !isNode;
    }
    for (int i = 0; i < identify.size(); i++)
        vertex_i[i] = vertex_i[identify[i]];

    vertices.clear();
    vertices.reserve((res + 1) * res / 2 - c);

    int i = 0;
    for (float y = 0; y < res; y++) {
        for (float x = 0; x < res - y; x++) {
            if (identify[i] != i++)
                continue;

            glm::vec2 uv(x / (res - 1), y / (res - 1));

            vertices.push_back(Vertex{
                glm::vec3(2 * uv.x - 1 + _shift * uv.y, _stretch * (2 * uv.y - 1), 0),
                glm::vec3(0, 0, 1),
                uv,
                glm::vec2(0)
                });
        }
    }

    triangles.clear();

    springs.clear();

    area.clear();

    float s = _shift * _shift + 4 * _stretch * _stretch;
    float d1 = 2.f / (res - 1);
    float d2 = glm::sqrt(s) / (res - 1);
    float d3 = glm::sqrt(4 - 4 * _shift + s) / (res - 1);

    float A = _stretch * d1 * d1 / 2.f;

    int it = 0;
    for (GLuint y = 0; y < res - 1; y++, it++) {
        for (GLuint x = 0; x < res - y - 1; x++, it++) {
            GLuint i = vertex_i[it], j = vertex_i[it + 1], k = vertex_i[it + res - y];

            if (x + 2 >= res - y) {
                triangles.push_back(i);
                triangles.push_back(j);
                triangles.push_back(k);
                area.push_back(d1 * d2 / 2);

                springs.push_back(spring_t{ i, j, d1 });
                springs.push_back(spring_t{ i, k, d2 });
                springs.push_back(spring_t{ j, k, d3 });

                continue;
            }

            GLuint l = vertex_i[it + res - y + 1];

            triangles.push_back(i);
            triangles.push_back(j);
            triangles.push_back(l);
            area.push_back(A);

            triangles.push_back(i);
            triangles.push_back(l);
            triangles.push_back(k);
            area.push_back(A);

            springs.push_back(spring_t{ i, j, d1 });
            springs.push_back(spring_t{ i, k, d2 });
            springs.push_back(spring_t{ j, k, d3 });
        }
    }

    //for (GLuint y = 0; y < res_y - 1; y++)
    //    springs.push_back(spring_t{ vertex_i[y * res_x], vertex_i[(y + 1) * res_x], d2 });
    //
    //for (GLuint x = 0; x < res_x - 1; x++)
    //    springs.push_back(spring_t{ vertex_i[x], vertex_i[x + 1], d1 });

    edgeSprings.clear();
    return;

    for (GLuint i : edge) {
        unsigned char flag;
        flag = i < res;
        flag |= 0 << 1;
        flag |= 0 << 2; // fix later

        switch (flag)
        {
        case 1: // bottom
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res], d2 });
            break;
        case 2: // left
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + 1], d2 });
            break;
        case 4: // right (not quite right)
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - 1], d2 }); 
            break;

        case 3:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res + 1], d3 });
            break;
        case 5:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i + res - 1], d3 });
            break;
        case 6:
            edgeSprings.push_back(spring_t{ vertex_i[i], vertex_i[i - 1], d2 });
            break;

        default:
            std::cout << "bad edge element: " << i << ". flag: " << (int)flag << std::endl;
        }
    }
}

void Mesh::init333(GLuint res, std::vector<GLuint>& identify, std::vector<GLuint>& edge) {
    init442(res, identify, edge, 1, glm::sqrt(3) / 2);
}

void Mesh::init632(GLuint res, std::vector<GLuint>& identify, std::vector<GLuint>& edge) {
    init442(res, identify, edge, -1, 3.f / 4);
}