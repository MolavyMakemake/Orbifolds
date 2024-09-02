#include"Mesh.h"
#include <glm/geometric.hpp>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices,
    const std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Delete() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Mesh::RecalculateNormals() {
    float *references = new float[vertices.size()]();

    for (int i = 0; i < indices.size(); i += 3) {
        
        int i0 = indices[i],
            i1 = indices[i + 1],
            i2 = indices[i + 2];

        glm::vec3 pos0 = vertices[i0].position;
        glm::vec3 pos1 = vertices[i1].position;
        glm::vec3 pos2 = vertices[i2].position;

        glm::vec3 normal = glm::cross(pos1 - pos0, pos2 - pos0);


        if (glm::dot(normal, pos0) < 0)
            normal = -normal;



        vertices[i0].normal += normal;
        vertices[i1].normal += normal;
        vertices[i2].normal += normal;

        references[i0]++;
        references[i1]++;
        references[i2]++;
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i].normal = glm::normalize(vertices[i].normal / references[i]);
    }

    Delete();
    Generate();
}