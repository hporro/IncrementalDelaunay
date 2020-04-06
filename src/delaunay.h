#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <vector>
#include <glm/glm.hpp>

typedef glm::vec2 Vec2;

class Triangle;


class Vertex {
public:
    Vertex(Vec2 v, Triangle* t);
    Vertex();
    Vec2 pos;
    Triangle* triangle;
    void print();
};

class Triangle {
public:
    Triangle(Vertex* v1,Vertex* v2,Vertex* v3,Triangle* t1,Triangle* t2,Triangle* t3);
    Triangle(Vec2 p1, Vec2 p2, Vec2 p3);
    Triangle();
    Vertex *v1,*v2,*v3;
    Triangle *t1,*t2,*t3;
    bool isInside(Vec2 p);
    bool isInEdge(Vec2 p);
    void print();
};

class Triangulation {
public:
    Triangulation(std::vector<Vec2> points, Triangle* t); //makes a triangulation of points
    void addPoint(Vec2 point);
    void addPointInside(Vec2 &point, Triangle* t);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    void draw(); // refills buffers
    void drawCached(); // draws the same buffers from the prev frame
    void print(); // prints using std::cout
    std::vector<Vec2> points;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};

#endif