#include <iostream>

#include "delaunay.h"
#include "constants.h"

//HELPER FUNCTIONS
float crossa(Vec2 a, Vec2 b){
    return a.x*b.y-a.y*b.x;
}

bool isLeft(Vec2 a, Vec2 b){
    return crossa(a,b) > -EPS;
}

bool isRight(Vec2 a, Vec2 b){
    return crossa(a,b) < EPS;
}

bool mightBeLeft(Vec2 a, Vec2 b){
    return crossa(a,b) >= -EPS;
}

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
    Vec2 a1 = p1-p2; Vec2 b1 = p-p2;
    Vec2 a2 = p2-p3; Vec2 b2 = p-p3;
    Vec2 a3 = p3-p1; Vec2 b3 = p-p1;
    if(mightBeLeft(p2-p1,p-p1) && mightBeLeft(p3-p2,p-p2) && mightBeLeft(p1-p3,p-p3))
    if( (a1.x>=b1.x && a1.y>=b1.y) || (a2.x>=b2.x && a2.y>=b2.y) || (a3.x>=b3.x && a3.y>=b3.y) ) return true;
    return false;
}

Triangulation::Triangulation(std::vector<Vec2> points, int numP, Vec2 p0, Vec2 p1, Vec2 p2) {
    vertices = std::vector<Vertex>(numP+3); // num of vertices
    triangles = std::vector<Triangle>(numP*2+1); // 2(n+3) - 2 - 3 = 2n+1 // num of faces

    vertices[0] = Vertex(p0);
    vertices[1] = Vertex(p1);
    vertices[2] = Vertex(p2);
    triangles[0] = Triangle(0,1,2,-1,-1,-1);
    assert(isCCW(0));

    vcount = 3;
    tcount = 1;

    for(int i=0;i<points.size();i++){
        addPoint(points[i]);
    }
}
bool Triangulation::integrity(int t){
    int t0 = triangles[t].t0;
    int t1 = triangles[t].t1;
    int t2 = triangles[t].t2;

    bool a=true,b=true,c=true;

    if(t0!=-1) a = (t==triangles[t0].t0) || (t==triangles[t0].t1) || (t==triangles[t0].t2);
    if(t1!=-1) b = (t==triangles[t1].t0) || (t==triangles[t1].t1) || (t==triangles[t1].t2);
    if(t2!=-1) c = (t==triangles[t2].t0) || (t==triangles[t2].t1) || (t==triangles[t2].t2);

    return a&&b&&c;
}
void Triangulation::addPointInside(Vec2 v, int tri_index){
    assert(isCCW(tri_index));
    assert(isInside(tri_index,v));
    assert(integrity(tri_index));

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
    
    if(t1!=-1){
        if(triangles[t1].t0==f)
        triangles[t1].t0 = f1;
        if(triangles[t1].t1==f)
        triangles[t1].t1 = f1;
        if(triangles[t1].t2==f)
        triangles[t1].t2 = f1;
    }

    if(t2!=-1){
        if(triangles[t2].t0==f)
        triangles[t2].t0 = f2;
        if(triangles[t2].t1==f)
        triangles[t2].t1 = f2;
        if(triangles[t2].t2==f)
        triangles[t2].t2 = f2;
    }

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
        this->incount++;
        addPointInside(p,tri_index);
        return;
    }
    int t1=-1,t2=-1;
    for(int i=0;i<tcount;i++){
        if((t1 == -1) && isInEdge(i,p)){
            t1 = i;
            continue;
        }
        if((t2 == -1) && isInEdge(i,p)){
            t2 = i;
        }
    }
    if((t1!=-1) && (t2!=-1)){
        this->edgecount++;
        addPointInEdge(p,t1,t2);
        return;
    }
    else if((t1!=-1) && (t2==-1)){
        this->oedgecount++;
        addPointInEdge(p,t1);
    }
}
void Triangulation::addPointInEdge(Vec2 v, int t1, int t2){
    assert(isCCW(t1)&&isCCW(t2));
    assert(isInEdge(t1,v)&&isInEdge(t2,v));
    assert(integrity(t1)&&integrity(t2));
    int f10,f20,f11,f21,f12,f22,p10,p20,p11,p21,p12,p22;
    
    p10 = triangles[t1].v0;
    p11 = triangles[t1].v1;
    p12 = triangles[t1].v2;
    f10 = triangles[t1].t0;
    f11 = triangles[t1].t1;
    f12 = triangles[t1].t2;
    
    p20 = triangles[t2].v0;
    p21 = triangles[t2].v1;
    p22 = triangles[t2].v2;
    f20 = triangles[t2].t0;
    f21 = triangles[t2].t1;
    f22 = triangles[t2].t2;

    int t3 = tcount++;
    int t4 = tcount++;
    int p = vcount++;
    vertices[p] = Vertex(v);

    if(f10 == t2){
        triangles[t3] = Triangle(p,p12,p10,f11,t1,t4);
        triangles[t1].v2 = p;
        triangles[t1].t1 = t3;
    }
    else if(f11 == t2){
        triangles[t3] = Triangle(p,p10,p11,f12,t1,t4);
        triangles[t1].v0 = p;
        triangles[t1].t2 = t3;
    }
    else if(f12 == t2){
        triangles[t3] = Triangle(p,p11,p12,f10,t1,t4);
        triangles[t1].v1 = p;
        triangles[t1].t0 = t3;
    }
    if(f20 == t1){
        triangles[t4] = Triangle(p,p20,p21,f22,t3,t2);
        triangles[t2].v1 = p;
        triangles[t2].t2 = t4;
    }
    else if(f21 == t1){
        triangles[t4] = Triangle(p,p21,p22,f20,t3,t2);
        triangles[t2].v2 = p;
        triangles[t2].t0 = t4;
    }
    else if(f22 == t1){
        triangles[t4] = Triangle(p,p22,p20,f21,t3,t2);
        triangles[t2].v0 = p;
        triangles[t2].t1 = t4;
    }
}
void Triangulation::addPointInEdge(Vec2 v, int t){
    assert(isCCW(t));
    assert(isInEdge(t,v));
    assert(integrity(t));
    int f0 = triangles[t].t0;
    int f1 = triangles[t].t1;
    int f2 = triangles[t].t2;

    int p0 = triangles[t].v0;
    int p1 = triangles[t].v1;
    int p2 = triangles[t].v2;
    
    int t1 = tcount++;
    int p = vcount++;

    vertices[p] = Vertex(v);
    triangles[t1] = Triangle(p0,p1,p,t,f1,f2);
    triangles[t].v0 = p;
    triangles[t].t2 = t1;
}
void Triangulation::print(){
    for(int i=0;i<tcount;i++){
        std::cout << "Triangle " << i << ":\n";
        std::cout << "P0: " << vertices[triangles[i].v0].pos.x << " " << vertices[triangles[i].v0].pos.y << std::endl;
        std::cout << "P1: " << vertices[triangles[i].v1].pos.x << " " << vertices[triangles[i].v1].pos.y << std::endl;
        std::cout << "P2: " << vertices[triangles[i].v2].pos.x << " " << vertices[triangles[i].v2].pos.y << std::endl;
    }
}
void Triangulation::print_ind(){
    for(int i=0;i<tcount;i++){
        std::cout << "Triangle " << i << ": ";
        std::cout << triangles[i].t0 << " " << triangles[i].t1 << " " << triangles[i].t2 << std::endl;
    }
}
bool Triangulation::isCCW(int f){
    Vec2 p0 = vertices[triangles[f].v0].pos;
    Vec2 p1 = vertices[triangles[f].v1].pos;
    Vec2 p2 = vertices[triangles[f].v2].pos;
    
    if(crossa(p0,p1)+crossa(p1,p2)+crossa(p2,p0)>-EPS) return true;
    
    return false;
}
void Triangulation::flip(int t1, int t2){
    int p1,p2,pa,pb,ta1,tb1,ta2,tb2;
    if(triangles[t1].t0==t2){
        p1 = triangles[t1].v0;
        pb = triangles[t1].v1;
        pa = triangles[t1].v2;
        tb1 = triangles[t1].t1;
        ta1 = triangles[t1].t2;
    }
    if(triangles[t1].t1==t2){
        p1 = triangles[t1].v1;
        pb = triangles[t1].v2;
        pa = triangles[t1].v0;
        tb1 = triangles[t1].t2;
        ta1 = triangles[t1].t0;
    }
    if(triangles[t1].t2==t2){
        p1 = triangles[t1].v2;
        pb = triangles[t1].v0;
        pa = triangles[t1].v1;
        tb1 = triangles[t1].t0;
        ta1 = triangles[t1].t1;
    }
    if(triangles[t2].t0==t1){
        p2 = triangles[t2].v0;
        ta2 = triangles[t1].t1;
        tb2 = triangles[t1].t2;
    }
    if(triangles[t2].t1==t1){
        p2 = triangles[t2].v1;
        ta2 = triangles[t1].t2;
        tb2 = triangles[t1].t0;
    }
    if(triangles[t2].t2==t1){
        p2 = triangles[t2].v2;
        ta2 = triangles[t1].t0;
        tb2 = triangles[t1].t1;
    }
    triangles[t1] = Triangle(pb,p2,p1,t2,ta1,ta2);
    triangles[t2] = Triangle(pa,p1,p2,t1,tb2,tb1);
}