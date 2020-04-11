#include <iostream>

#include "delaunay.h"
#include "constants.h"

//VERTEX IMPL
Vertex::Vertex(Vec2 v, int t) : pos(v), tri_index(t){}
Vertex::Vertex(Vec2 v) : pos(v){}
Vertex::Vertex(){}
void Vertex::print(){
    std::cout << pos.x << " " << pos.y << std::endl;
}

//TRIANGLE IMPL
Triangle::Triangle(int v0,int v1,int v2,int t0,int t1,int t2):v0(v0),v1(v1),v2(v2),t0(t0),t1(t1),t2(t2){}
Triangle::Triangle(){}

float crossa(Vec2 a, Vec2 b){
    return a.x*b.y-a.y*b.x;
}

bool isLeft(Vec2 a, Vec2 b){
    return crossa(a,b) > 0;
}

bool mightBeLeft(Vec2 a, Vec2 b){
    return crossa(a,b) >= 0;
}

//TRIANGULATION IMPL
bool Triangulation::isInside(int t, Vec2 p){
    Vec2 p1 = vertices[triangles[t].v0].pos;
    Vec2 p2 = vertices[triangles[t].v1].pos;
    Vec2 p3 = vertices[triangles[t].v2].pos;
    // b-a goes from a to b
    if(isLeft(p2-p1,p-p1) && isLeft(p3-p2,p-p2) && isLeft(p1-p3,p-p3)) return true;
    return false;
}

bool Triangulation::isInEdge(int t, Vec2 p){
    if(isInside(t,p))return false; // in our context this will never happend, so we can skip this check
    Vec2 p1 = vertices[triangles[t].v0].pos;
    Vec2 p2 = vertices[triangles[t].v1].pos;
    Vec2 p3 = vertices[triangles[t].v2].pos;
    // b-a goes from a to b
    if(mightBeLeft(p2-p1,p-p1) && mightBeLeft(p3-p2,p-p2) && mightBeLeft(p1-p3,p-p3)) return true;
    return false;
}

Triangulation::Triangulation(std::vector<Vec2> points, int numP, Vec2 p0, Vec2 p1, Vec2 p2) {
    //TODO: Assert T(p1, p2, p3) is ccw

    vertices = std::vector<Vertex>(numP+3); // num of vertices
    triangles = std::vector<Triangle>(numP*2+1); // 2(n+3) - 2 - 3 = 2n+1 // num of faces

    vertices[0] = Vertex(p0);
    vertices[1] = Vertex(p1);
    vertices[2] = Vertex(p2);
    triangles[0] = Triangle(0,1,2,-1,-1,-1);

    vcount = 3;
    tcount = 1;

    for(int i=0;i<points.size();i++){
        addPoint(points[i]);
    }
}
void Triangulation::addPointInside(Vec2 v, int tri_index){
    int f = tri_index;
    int f1 = tcount++;
    int f2 = tcount++;

    int p = vcount++;

    int p0 = triangles[f].v0;
    int p1 = triangles[f].v1;
    int p2 = triangles[f].v2;
    int t0 = triangles[f].t0;
    int t1 = triangles[f].t1;
    int t2 = triangles[f].t2;

    triangles[f1] = Triangle(p,p2,p0,t1,f2,f);
    triangles[f2] = Triangle(p,p0,p1,t2,f,f1);
    
    triangles[f].v0 = p;
    triangles[f].t1 = f1;
    triangles[f].t2 = f2;

    vertices[p] = Vertex(v);
}
void Triangulation::addPoint(Vec2 p){
    int tri_index = -1;
    for(int i=0;i<tcount;i++){
        if(isInside(i,p)){
            tri_index = i;
            break;
        }
    }
    if(tri_index!=-1){
        addPointInside(p,tri_index);
        return;
    }
    int t1=-1,t2=-1;
    for(int i=0;i<tcount;i++){
        if(t1 == -1 && isInEdge(i,p)){
            t1 = i;
            continue;
        }
        if(t2 == -1 && isInEdge(i,p)){
            t2 = i;
            break;
        }
    }
    if(t1!=-1 && t2!=-1){
        addPointInEdge(p,t1,t2);
    }
    else if(t1!=-1 && t2==-1){
        veocount++;
    }
    else{
        //std::cout << p.x << " " << p.y << std::endl;
    }
}
void Triangulation::addPointInEdge(Vec2 p, int t1, int t2){
    vecount++;
}
void Triangulation::print(){
    for(int i=0;i<tcount;i++){
        std::cout << "Triangle " << i << ":\n";
        std::cout << "P0: " << vertices[triangles[i].v0].pos.x << " " << vertices[triangles[i].v0].pos.y << std::endl;
        std::cout << "P1: " << vertices[triangles[i].v1].pos.x << " " << vertices[triangles[i].v1].pos.y << std::endl;
        std::cout << "P2: " << vertices[triangles[i].v2].pos.x << " " << vertices[triangles[i].v2].pos.y << std::endl;
    }
}
bool Triangulation::isCCW(int f){
    Vec2 p0 = vertices[triangles[f].v0].pos;
    Vec2 p1 = vertices[triangles[f].v1].pos;
    Vec2 p2 = vertices[triangles[f].v1].pos;
    
    if(isLeft(p1-p0,p2-p0) && isLeft(p2-p1,p0-p1) && isLeft(p0-p2,p1-p2)) return true;
    return false;
}
