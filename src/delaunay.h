#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <vector>
#include <glm/glm.hpp>

class Vertex {
public:
    glm::vec2* pos;
    int* he;
};

class Triangle {
public:
    int ind; // triangle index
    bool isInside(glm::vec2 *p);
};

class Half_Edge {
public:
    void flip();
    int *twin; // twin half_edge index
    int *vert; // vertex index
};

class Triangulation {
public:
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    void draw(); // refills buffers
    void drawCached(); // draws the same buffers from the prev frame
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
    std::vector<Half_Edge*> h_edges;
};

#endif