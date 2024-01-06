#include "triangle_mesh.h"

TriangleMesh::TriangleMesh() {

    vector<float> data = {
//normal    x,     y,    z,
        -1.0f, -1.0f, 0.0f, //Bottom left
         1.0f, -1.0f, 0.0f, //Bottom right
         1.0f,  1.0f, 0.0f, //Top right
        -1.0f,  1.0f, 0.0f, //Top left
    };

    vertex_count = 6;

    vector<int> elementIndicies = { 0, 1, 2, 0, 2, 3 };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
    glEnableVertexAttribArray(0);


    //element buffer
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementIndicies.size() * sizeof(int), elementIndicies.data(), GL_STATIC_DRAW);
}

void TriangleMesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}