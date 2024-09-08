#include "Mesh.h"

#define P1_RADIUS   1.0f
#define PM_RADIUS   .2f

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

//glm::vec3 p1_pos(float x, float y) { return glm::vec3((pi * P1_RADIUS - glm::cos(pi * y)) * (x - .5f), y - .5f, 0); }
//glm::vec3 p2_pos(float x, float y) { return glm::vec3(.5f * x, y, 0); }
//glm::vec3 pm_pos(float x, float y) { return glm::vec3( pi * (PM_RADIUS + y) * (x - .5f), y, 0); }
//glm::vec3 cm_pos(float x, float y) { return glm::vec3(5.f * x - 2.5f, .5f * y - .25f, 0); }
glm::vec3 p1_pos(float x, float y) { return glm::vec3(2.f * x - 1.f, 2.f * y - 1.f, 0); }
glm::vec3 p2_pos(float x, float y) { return glm::vec3(2.f * x - 1.f, 2.f * y - 1.f, 0); }
glm::vec3 pm_pos(float x, float y) { return glm::vec3(2.f * x - 1.f, 2.f * y - 1.f, 0); }
glm::vec3 cm_pos(float x, float y) { return glm::vec3(2.f * x - 1.f, 2.f * y - 1.f, 0); }

glm::vec3 p3_pos(float x, float y) { return glm::vec3((phi - .5f) * (2.f * x - 1.f) - .5f * (2.f * y - 1.f), 2.f * y - 1.f, 0); }

void Mesh::computeDomain() {
    edge.clear();
    std::vector<GLuint> identify;

    iteration = 0;
    GLuint res_x = resolution.x, res_y = resolution.y;

    switch (domain)
    {
    case MESH_P1:
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 0; y < res_y - 1; y++)
            identify[res_x * (y + 1) - 1] = y * res_x;

        identify[res_x * res_y - 1] = 0;

        computeMesh(res_x, res_y, SHAPE_2222, identify, p1_pos);
        break;

    case MESH_PM:
        res_x *= 2;
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

        for (GLuint x = 0; x < res_x; x++) {
            edge.push_back(x);
            edge.push_back(res_x * (res_y - 1) + x);
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, pm_pos);

        break;

    case MESH_PG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 1; y < res_y - 1; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        identify[res_x - 1] = 0;
        identify[res_x * res_y - 1] = 0;

        computeMesh(res_x, res_y, SHAPE_2222, identify);
        break;

    case MESH_CM:
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        for (GLuint x = 0; x < res_x; x++) {
            edge.push_back(x);
            edge.push_back(res_x * (res_y - 1) + x);
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, cm_pos);
        par.explode = 0.01f;
        par.pressure = 0.01f;
        break;

    case MESH_P2: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 1; y < res_y / 2; y++) {
            identify[res_x * (res_y - y)] = res_x * y;
            identify[res_x * (res_y - y + 1) - 1] = res_x * (y + 1) - 1;
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, p2_pos);
        par.pressure = 0.15f;
        break;

    case MESH_PMM:
        identify = arange(res_x * res_y);
        computeMesh(res_x, res_y, SHAPE_2222, identify);
        par.pressure = 0;
        break;

    case MESH_PMG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x / 2; x++) {
            identify[res_x - 1 - x] = x;
            identify[res_x * res_y - 1 - x] = res_x * (res_y - 1) + x;
        }

        for (GLuint y = 0; y < res_y; y++) {
            edge.push_back(y * res_x);
            edge.push_back((y + 1) * res_x - 1);
        }

        edge.push_back(res_x / 2);
        edge.push_back(res_x * (res_x - 1) + res_x / 2);

        computeMesh(res_x, res_y, SHAPE_2222, identify);
        par.pressure = 0.2f;
        break;

    case MESH_PGG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * res_y - 1 - x] = x;
        
        for (GLuint y = 1; y < res_y; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        computeMesh(res_x, res_y, SHAPE_2222, identify);
        break;

    case MESH_CMM: // Not verified
    {
        int N = (res_x + 1) * res_x / 2;
        identify = arange(N);

        int i = -1, j = N - 1;
        for (GLuint y = 0; y < res_x / 2; y++) {
            i += res_x - y;
            j -= y;
            identify[j] = i;
        }

        edge.push_back(j - res_x / 2);

        i = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            edge.push_back(x);
            edge.push_back(i);
            i += res_x - x;
        }

        edge.push_back(0);

        computeMesh(res_x, res_y, SHAPE_442, identify);
        break;
    }

    case MESH_P3:
        identify = arange(res_x * res_x);

        for (GLuint i = 1; i < res_x - 1; i++) {
            identify[res_x * i] = i;
            identify[res_x * (res_x - 1) + i] = (i + 1) * res_x - 1;
        }
        
        edge.push_back(0);
        edge.push_back(res_x - 1);
        edge.push_back(res_x * res_x - 1);

        identify[res_x * (res_x - 1)] = res_x - 1;

        computeMesh(res_x, res_x, SHAPE_2222, identify, p3_pos);
        break;

    case MESH_P3M1:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_333, identify);
        break;

    case MESH_P31M:
    {
        int N = (res_x + 1) * res_x / 2;
        int i, j;

        identify = arange(N);

        j = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            identify[j] = x;
            edge.push_back(j - 1);
            j += res_x - x;
        }

        edge.push_back(N - 1);
        edge.push_back(0);

        computeMesh(res_x, res_y, SHAPE_662, identify);
        par.pressure = 0.1f;
        par.explode = 0.001f;
        break;
    }

    case MESH_P4: // Not verified
        identify = arange(res_x * res_x);

        for (GLuint i = 1; i < res_x - 1; i++) {
            identify[res_x * i] = i;
            identify[res_x * (res_x - 1) + i] = (i + 1) * res_x - 1;
        }

        identify[res_x * (res_x - 1)] = res_x - 1;

        edge.push_back(0);
        edge.push_back(res_x - 1);
        edge.push_back(res_x * res_x - 1);

        computeMesh(res_x, res_x, SHAPE_2222, identify);
        break;

    case MESH_P4M:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_442, identify);
        break;

    case MESH_P4G: // Not verified
    {
        identify = arange((res_x + 1) * res_x / 2);

        GLuint j = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            identify[j] = x;
            edge.push_back(j - 1);
            j += res_x - x;
        }

        edge.push_back((res_x + 1) * res_x / 2 - 1);
        edge.push_back(0);

        computeMesh(res_x, res_y, SHAPE_442, identify);
        break;
    }

    case MESH_P6: // Not verified
    {
        int N = (res_x + 1) * res_x / 2;
        int i, j;

        identify = arange(N);

        j = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            identify[j] = x;
            j += res_x - x;
        }

        i = res_x - 1;
        j = N - 1;
        for (GLuint y = 1; y < res_x / 2; y++) {
            i += res_x - y;
            j -= y;
            identify[j] = i;
        }

        edge.push_back(0);
        edge.push_back(res_x - 1);
        edge.push_back(j - res_x / 2);

        computeMesh(res_x, res_y, SHAPE_662, identify);
        par.pressure = 0.3f;
        par.explode = 0.01f;
        break;
    }

    case MESH_P6M:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_632, identify);
        par.pressure = 0;
        break;

    default:
        break;
    }

    velocity.assign(vertices.size(), glm::vec3(0));
}