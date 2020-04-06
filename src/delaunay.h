#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <vector>
#include <glm/glm.hpp>

typedef glm::vec2 Vec2;
class Triangle;


class Vertex {
public:
    Vec2* pos;
    Triangle* triangle;
};

class Triangle {
public:
    Vertex *a,*b,*c;
    Triangle *t1,*t2,*t3;
    bool isInside(Vec2 *p);
    bool isInEdge(Vec2 *p);
};

class Triangulation {
public:
    Triangulation(std::vector<Vec2> points); //makes a triangulation of points
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    void draw(); // refills buffers
    void drawCached(); // draws the same buffers from the prev frame
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;
};

#endif