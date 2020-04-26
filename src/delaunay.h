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
    int v[3]; // indices to the vertices vector
    int t[3]; // indices to the triangles vector
};

class Triangulation {
public:
    Triangulation(std::vector<Vec2> points, int numP, Vec2 p1, Vec2 p2, Vec2 p3); //makes a triangulation out of a list of points and a triangle. numP is to allocate memory for numP vertices
    Triangulation(std::vector<Vec2> points, int numP);
    Triangulation(){}
    bool isInside(int t, Vec2); //checks if a Vec2 is inside the triangle in the index t
    bool isInEdge(int t, Vec2); //checks if a Vec2 is in a edge of a triangle
    void print(); // prints the triangulation to standard output
    void print_ind(); // prints connectivity
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    int vcount = 0;
    int tcount = 0;
    int incount = 0;
    int edgecount = 0;
    int oedgecount = 0;
    void addPoint(Vec2 point);
    void addPointInside(Vec2 point,int);
    void addPointInEdge(Vec2 point, int t1, int t2);
    void addPointInEdge(Vec2 point, int t);
    bool isCCW(int f); // check if a triangle, in the position f of the triangles array, is ccw
    void flip(int t1, int t2);
    bool integrity(int t);
    void legalize(int t1, int t2);
    bool areConnected(int,int);
    bool frontTest(int);
};

float crossa(Vec2 a, Vec2 b);
bool isLeft(Vec2 a, Vec2 b);
bool isRight(Vec2 a, Vec2 b);
bool mightBeLeft(Vec2 a, Vec2 b);

#endif