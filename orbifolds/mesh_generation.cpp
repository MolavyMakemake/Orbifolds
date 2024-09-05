#include "Mesh.h"

#define P1_RADIUS   .7f
#define PM_RADIUS   .7f

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

const float pi = 6.2831853f;

float p1_len(float y) { return pi * P1_RADIUS - 4 * glm::cos(pi * y); }
float p2_len(float y) { return .5f; }
float pm_len(float y) { return pi * (PM_RADIUS + 2 * y); }
float cm_len(float y) { return 5.f; }

void Mesh::computeDomain() {
    std::vector<GLuint> identify;
    std::vector<GLuint> edge;

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

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge, p1_len);
        break;

    case MESH_PM:
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x - 1 + y * res_x] = y * res_x;

        for (GLuint x = 0; x < res_x - 1; x++) {
            edge.push_back(x);
            edge.push_back(res_x * (res_x - 1) + x);
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge, pm_len);
        break;

    case MESH_PG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 1; y < res_y - 1; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        identify[res_x - 1] = 0;
        identify[res_x * res_y - 1] = 0;

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge);
        break;

    case MESH_CM:
        identify = arange(res_x * res_y);

        for (GLuint y = 0; y < res_y; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge);
        break;

    case MESH_P2: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x; x++)
            identify[res_x * (res_y - 1) + x] = x;

        for (GLuint y = 1; y < res_y / 2; y++) {
            identify[res_x * (res_y - y)] = res_x * y;
            identify[res_x * (res_y - y + 1) - 1] = res_x * (y + 1) - 1;
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge, p2_len);
        break;

    case MESH_PMM:
        identify = arange(res_x * res_y);
        computeMesh(res_x, res_y, SHAPE_2222, identify, edge);
        break;

    case MESH_PMG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x / 2; x++) {
            identify[res_x - 1 - x] = x;
            identify[res_x * res_y - 1 - x] = res_x * (res_y - 1) + x;
        }

        for (int y = 0; y < res_y; y++) {
            for (int x = 0; x < res_x; x++)
                std::cout << identify[y * res_x + x] << " ";
            std::cout << std::endl;
        }

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge);
        break;

    case MESH_PGG: // Not verified
        identify = arange(res_x * res_y);

        for (GLuint x = 0; x < res_x - 1; x++)
            identify[res_x * res_y - 1 - x] = x;
        
        for (GLuint y = 1; y < res_y; y++)
            identify[res_x * (res_y - y) - 1] = y * res_x;

        computeMesh(res_x, res_y, SHAPE_2222, identify, edge);
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

        computeMesh(res_x, res_y, SHAPE_442, identify, edge);
        break;
    }

    case MESH_P3:
        identify = arange(res_x * res_x);

        for (GLuint i = 1; i < res_x - 1; i++) {
            identify[res_x * i] = i;
            identify[res_x * (res_x - 1) + i] = (i + 1) * res_x - 1;
        }

        identify[res_x * (res_x - 1)] = res_x - 1;

        computeMesh(res_x, res_x, SHAPE_2222, identify, edge);
        break;

    case MESH_P3M1:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_333, identify, edge);
        break;

    case MESH_P31M:
    {
        int N = (res_x + 1) * res_x / 2;
        int i, j;

        identify = arange(N);

        j = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            identify[j] = x;
            j += res_x - x;
        }

        computeMesh(res_x, res_y, SHAPE_632, identify, edge);
        break;
    }

    case MESH_P4: // Not verified
        identify = arange(res_x * res_x);

        for (GLuint i = 1; i < res_x - 1; i++) {
            identify[res_x * i] = i;
            identify[res_x * (res_x - 1) + i] = (i + 1) * res_x - 1;
        }

        identify[res_x * (res_x - 1)] = res_x - 1;

        computeMesh(res_x, res_x, SHAPE_2222, identify, edge);
        break;

    case MESH_P4M:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_442, identify, edge);
        break;

    case MESH_P4G: // Not verified
    {
        identify = arange((res_x + 1) * res_x / 2);

        GLuint j = res_x;
        for (GLuint x = 1; x < res_x; x++) {
            identify[j] = x;
            j += res_x - x;
        }

        computeMesh(res_x, res_y, SHAPE_442, identify, edge);
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

        i = 0;
        for (int y = 0; y < res_x; y++) {
            for (int x = 0; x < res_x - y; x++)
                std::cout << hex(identify[i++]) << " ";

            std::cout << std::endl;
        }

        computeMesh(res_x, res_y, SHAPE_632, identify, edge);
        break;
    }

    case MESH_P6M:
        identify = arange((res_x + 1) * res_x / 2);
        computeMesh(res_x, res_y, SHAPE_632, identify, edge);

    default:
        break;
    }
}