#include <iostream>

#include "delaunay.h"

//VERTEX IMPL
Vertex::Vertex(Vec2 v, Triangle* t) : pos(v), triangle(t){}
Vertex::Vertex(){}
void Vertex::print(){
    std::cout << pos.x << " " << pos.y << std::endl;
}

//TRIANGLE IMPL
Triangle::Triangle(Vec2 p1, Vec2 p2, Vec2 p3){
    v1 = new Vertex();
    v2 = new Vertex();
    v3 = new Vertex();

    v1->pos = p1;
    v2->pos = p2;
    v3->pos = p3;

    v1->triangle = &(*this);
    v2->triangle = &(*this);
    v3->triangle = &(*this);
}
Triangle::Triangle(Vertex* v1,Vertex* v2,Vertex* v3,Triangle* t1,Triangle* t2,Triangle* t3):v1(v1),v2(v2),v3(v3),t1(t1),t2(t2),t3(t3){}
Triangle::Triangle(){}
void Triangle::print(){
    std::cout << "v1: "; v1->print();
    std::cout << "v2: "; v2->print();
    std::cout << "v3: "; v3->print();
}

bool Triangle::isInside(Vec2 p){
    Vec2 p1 = v1->pos;
    Vec2 p2 = v2->pos;
    Vec2 p3 = v3->pos;

    // b-a goes from a to b

    if(true){
        return true;
    }

    return false;
}

//TRIANGULATION IMPL
Triangulation::Triangulation(std::vector<Vec2> points, Triangle* t) : points(points) {
    t->t1 = nullptr;
    t->t2 = nullptr;
    t->t3 = nullptr;

    triangles.push_back(*t);

    for(int i=0;i<points.size();i++){
        addPoint(points[i]);
    }
}
void Triangulation::addPointInside(Vec2 &p, Triangle* t){
    vertices.push_back(Vertex(p,t));
    
    Vertex *v = &vertices[vertices.size()-1];
    Vertex *v1 = t->v1;
    Vertex *v2 = t->v2;
    Vertex *v3 = t->v3;

    Triangle *t1 = t->t1;
    Triangle *t2 = t->t2;
    Triangle *t3 = t->t3;

    triangles.push_back(Triangle());
    triangles.push_back(Triangle());
    Triangle* f3 = &triangles[triangles.size()-1];
    Triangle* f2 = &triangles[triangles.size()-2];

    t->t2 = f2;
    t->t3 = f3;
    t->v3 = v;

    f2->t1 = t2;
    f2->t2 = f3;
    f2->t3 = t;
    f2->v1 = v2;
    f2->v2 = v3;
    f2->v3 = v;

    f3->t1 = t3;
    f3->t2 = t;
    f3->t3 = f2;
    f3->v1 = v3;
    f3->v2 = v1;
    f3->v3 = v;
}

void Triangulation::addPoint(Vec2 p){
    Triangle* t = nullptr;
    for(int i=0;i<triangles.size();i++){
        if(triangles[i].isInside(p)){
            t = &triangles.at(i);
            break;
        }
    }
    if(t!=nullptr){
        addPointInside(p,t);
    }
}
void Triangulation::print(){
    for(int i=0;i<triangles.size();i++){
        std::cout << "Triangle " << i << ":" << std::endl; triangles[i].print();
    }
}