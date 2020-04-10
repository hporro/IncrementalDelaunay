#ifndef DELAUNAY_H
#define DELAUNAY_H

//#include <GL/gl3w.h>

#include <vector>
#include <glm/glm.hpp>

typedef glm::vec2 Vec2;

class Triangle;


class Vertex {
public:
    Vertex(Vec2 v, int t);
    Vertex();
    Vertex(Vec2);
    Vec2 pos;
    int tri_index;
    void print();
};

class Triangle {
public:
    Triangle(int v1,int v2,int v3,int t1,int t2,int t3);
    Triangle();
    int v0,v1,v2; // indices to the vertices vector
    int t0,t1,t2; // indices to the triangles vector
};

class Triangulation {
public:
    Triangulation(std::vector<Vec2> points, int numP, Vec2 p1, Vec2 p2, Vec2 p3); //makes a triangulation out of a list of points and a triangle. numP is to allocate memory for numP vertices
    bool isInside(int t, Vec2); //checks if a Vec2 is inside the triangle in the index t
    void print(); // prints the triangulation to standard output
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    int vcount = 0;
    int tcount = 0;
private:
    void addPoint(Vec2 point);
    void addPointInside(Vec2 point,int);
};

#endif