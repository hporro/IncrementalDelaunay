#include <iostream>

#include "delaunay.h"
#include "constants.h"

#include <queue>
#include <algorithm>
#include <csignal>

#include "utils.h"

#define __H_BREAKPOINT__ __asm__("int $3")

//VERTEX IMPL
Vertex::Vertex(Vec2 v, int t) : pos(v), tri_index(t){}
Vertex::Vertex(Vec2 v) : pos(v){}
Vertex::Vertex(){}
void Vertex::print(){
    std::cout << pos.x << " " << pos.y << std::endl;
}

//TRIANGLE IMPL
Triangle::Triangle(int v0,int v1,int v2,int t0,int t1,int t2){
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    t[0] = t0;
    t[1] = t1;
    t[2] = t2;
}
Triangle::Triangle(){}

//TRIANGULATION IMPL
bool Triangulation::isInside(int t, Vec2 p){
    if (t == -1)return 0;
    Vec2 p1 = vertices[triangles[t].v[0]].pos;
    Vec2 p2 = vertices[triangles[t].v[1]].pos;
    Vec2 p3 = vertices[triangles[t].v[2]].pos;
    // b-a goes from a to b
    if(isLeft(p2-p1,p-p1) && isLeft(p3-p2,p-p2) && isLeft(p1-p3,p-p3)) return true;
    return false;
}

bool Triangulation::validTriangle(int t){
    if(t==-1)return true;
    bool res = true;
    if(t>=tcount)res=false;
    if(t<-1)res=false;
    for(int i=0;i<3;i++)if(triangles[t].t[i]<-1)res=false;
    for(int i=0;i<3;i++)if(triangles[t].t[i]>tcount)res=false;
    for(int i=0;i<3;i++)if(triangles[t].v[i]<0)res=false;
    for(int i=0;i<3;i++)if(triangles[t].v[i]>vcount)res=false;

    for(int i=0;i<3;i++){
        if(triangles[t].v[i]==triangles[t].v[(i+1)%3])res=false;
        if(triangles[t].t[i]==triangles[t].t[(i+1)%3] && triangles[t].t[i]!=-1)res=false;
    }
    // if(!res) __H_BREAKPOINT__;
    return true;
}

bool Triangulation::isInEdge(int t, Vec2 p){
    Vec2 p1 = vertices[triangles[t].v[0]].pos;
    Vec2 p2 = vertices[triangles[t].v[1]].pos;
    Vec2 p3 = vertices[triangles[t].v[2]].pos;
    // b-a goes from a to b
    Vec2 a1 = p1-p2; Vec2 b1 = p-p2;
    Vec2 a2 = p2-p3; Vec2 b2 = p-p3;
    Vec2 a3 = p3-p1; Vec2 b3 = p-p1;
    if(mightBeLeft(p2-p1,p-p1) && mightBeLeft(p3-p2,p-p2) && mightBeLeft(p1-p3,p-p3))
    if( (a1.x>=b1.x && a1.y>=b1.y) || (a2.x>=b2.x && a2.y>=b2.y) || (a3.x>=b3.x && a3.y>=b3.y) ) return true;
    return false;
}

Triangulation::Triangulation(std::vector<Vec2> points, int numP, bool logSearch = true) :  doLogSearch(logSearch) {
    
    float minx = 100000;
    float miny = 100000;
    float maxx =-100000;
    float maxy =-100000;

    for(auto p: points){
        minx = std::min(minx,p.x);
        miny = std::min(miny,p.y);
        maxx = std::max(maxx,p.x);
        maxy = std::max(maxy,p.y);
    }
    
    a = std::max(maxx-minx,maxy-miny);

    p0 = Vec2(minx,miny) + Vec2(-a/10,-a/10);
    p1 = p0 + Vec2(a+2*a/10,0);
    p2 = p0 + Vec2(a+2*a/10,a+2*a/10);
    p3 = p0 + Vec2(0,a+2*a/10);

    maxVertices = numP+6;
    maxTriangles = numP*2+7;
    vertices = new Vertex[maxVertices]; // num of vertices
    triangles = new Triangle[maxTriangles]; // 2(n+6) - 2 - 3 = 2n+7 // num of faces

    vertices[0] = Vertex(p0);
    vertices[1] = Vertex(p1);
    vertices[2] = Vertex(p2);
    vertices[3] = Vertex(p3);

    triangles[0] = Triangle(0,1,2,-1,1,-1);
    triangles[1] = Triangle(0,2,3,-1,-1,0);

    vcount = 4;
    tcount = 2;

#if ASSERT_PROBLEMS
    assert(isCCW(0)&&isCCW(1));
    assert(frontTest(0));
#endif

    for(int i=0;i<(int)points.size();i++){
        delaunayInsertion(points[i]);
    }
}

bool Triangulation::integrity(int t){
    int t0 = triangles[t].t[0];
    int t1 = triangles[t].t[1];
    int t2 = triangles[t].t[2];

    bool a=true,b=true,c=true;

    if(t0!=-1) a = (t==triangles[t0].t[0]) || (t==triangles[t0].t[1]) || (t==triangles[t0].t[2]);
    if(t1!=-1) b = (t==triangles[t1].t[0]) || (t==triangles[t1].t[1]) || (t==triangles[t1].t[2]);
    if(t2!=-1) c = (t==triangles[t2].t[0]) || (t==triangles[t2].t[1]) || (t==triangles[t2].t[2]);

    // if(((a&&b)&&c)==false) __H_BREAKPOINT__;
    return (a&&b)&&c;
}

bool Triangulation::frontTest(int t){ // checks that every point is in the same index of a triangle that the triangle in front of it
    if(t==-1)return true;
    bool res = true;
    for(int i=0;i<3;i++){
        int v = triangles[t].v[i];
        int f = triangles[t].t[i];
        if(f!=-1)for(int j=0;j<3;j++){
            if(triangles[f].v[j]==v){
                res=false;
                std::cout << t << " " <<  v << " != " << f << " " << triangles[f].v[j] << std::endl; 
            }
        }
    }
    // if(!res) __H_BREAKPOINT__;
    return res;
}

void Triangulation::addPointInside(Vec2 v, int tri_index){
    remem();
    int f = tri_index;
    int f1 = tcount++;
    int f2 = tcount++;

    int p = vcount++;

    int p0 = triangles[f].v[0];
    int p1 = triangles[f].v[1];
    int p2 = triangles[f].v[2];
    int t1 = triangles[f].t[1];
    int t2 = triangles[f].t[2];

    triangles[f1] = Triangle(p,p2,p0,t1,f2,f);
    triangles[f2] = Triangle(p,p0,p1,t2,f,f1);
    
    if(t1!=-1){
        if(triangles[t1].t[0]==f)
        triangles[t1].t[0] = f1;
        if(triangles[t1].t[1]==f)
        triangles[t1].t[1] = f1;
        if(triangles[t1].t[2]==f)
        triangles[t1].t[2] = f1;
    }

    if(t2!=-1){
        if(triangles[t2].t[0]==f)
        triangles[t2].t[0] = f2;
        if(triangles[t2].t[1]==f)
        triangles[t2].t[1] = f2;
        if(triangles[t2].t[2]==f)
        triangles[t2].t[2] = f2;
    }

    triangles[f].v[0] = p;
    triangles[f].t[1] = f1;
    triangles[f].t[2] = f2;

    vertices[p] = Vertex(v);

    updateNextToMinOne(f);
    updateNextToMinOne(f1);
    updateNextToMinOne(f2);
    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
}

int Triangulation::findContainerTriangleLinearSearch(Vec2 p){
    for(int i=0;i<tcount;i++){
        if(isInside(i,p)){
            return i;
        }
    }
    return -1;
}

int Triangulation::findContainerTriangleSqrtSearch(Vec2 p, Vec2 initialPoint, int prop, int cameFrom){
    if(isInside(prop,p))return prop;

    int *visited = new int[tcount];
    for(int i=0;i<tcount;i++)visited[i]=0;

    Vec2 v = initialPoint;

    std::vector<int> q(10);
    q.push_back(prop);

    while(!q.empty()){
        int t = q[q.size()-1];
        if(isInside(t,p))return t;
        if(isInEdge(t,p))return t;
        q.pop_back();
        for(int i=0;i<3;i++){
            int f = triangles[t].t[i];
            if(f==-1)continue;
            Vec2 a = vertices[triangles[t].v[(i+1)%3]].pos;
            Vec2 b = vertices[triangles[t].v[(i+2)%3]].pos;
            if(
                (mightBeLeft(p-v,a-v) && mightBeLeft(b-v,p-v)) ||
                (mightBeLeft(p-v,b-v) && mightBeLeft(a-v,p-v))
            ){
                if(!visited[f]){
                    visited[f]=1;
                    q.push_back(f);
                }
            }
        }
    }

    delete[] visited;

    return -1;
}

bool Triangulation::delaunayInsertion(Vec2 p){
#if ASSERT_PROBLEMS
    assert(
        triangles[nextToMinOne].t[0]==-1 ||
        triangles[nextToMinOne].t[1]==-1 ||
        triangles[nextToMinOne].t[2]==-1
    );
#endif

    remem();

    Vec2 initialPoint = (vertices[triangles[nextToMinOne].v[0]].pos+vertices[triangles[nextToMinOne].v[1]].pos+vertices[triangles[nextToMinOne].v[2]].pos)/3;
    int tri_index;
    if(doLogSearch) tri_index = findContainerTriangleSqrtSearch(p,initialPoint,nextToMinOne,-1);
    else tri_index = findContainerTriangleLinearSearch(p);
    
    Vec2 points[] = {vertices[triangles[tri_index].v[0]].pos,vertices[triangles[tri_index].v[1]].pos,vertices[triangles[tri_index].v[2]].pos};

    for(int i=0;i<3;i++){ // we dont insert repeated points
        if((abs(p[0]-points[i][0])<IN_TRIANGLE_EPS) && (abs(p[1]-points[i][1])<IN_TRIANGLE_EPS))return false;
    }

    for(int i=0;i<3;i++){
        if(pointInSegment(p,points[(i+1)%3],points[(i+2)%3])){
            // insert a point in the i edge
            if(triangles[tri_index].t[i]==-1){
                addPointInEdge(p,tri_index);
                for(int j=0;j<3;j++){
                    legalize(tri_index,triangles[tri_index].t[j]);
                    legalize(tcount-1,triangles[tcount-1].t[j]);
                }
            }
            else{
                addPointInEdge(p,tri_index,triangles[tri_index].t[i]);
                for(int j=0;j<3;j++){
                    legalize(triangles[tri_index].t[i],triangles[triangles[tri_index].t[i]].t[j]);
                    legalize(tri_index,triangles[tri_index].t[j]);
                    legalize(tcount-1,triangles[tcount-1].t[j]);
                    legalize(tcount-2,triangles[tcount-2].t[j]);
                }
            }
            return true;
        }
    }
    
    if(tri_index!=-1){
        this->incount++;
        addPointInside(p,tri_index);
        int a = tri_index,b=tcount-1,c=tcount-2;
        for (int i = 0; i < 3; i++) {
            legalize(a, triangles[a].t[i]);
            legalize(b, triangles[b].t[i]);
            legalize(c, triangles[c].t[i]);
        }
        return true;
    }
    return true;
}

// gets the pair of vertex indices shared by two triangles
std::pair<int,int> Triangulation::getVerticesSharedByTriangles(int t1, int t2){
#if ASSERT_PROBLEMS
    assert(areConnected(t1,t2));
#endif
    for(int i=0;i<3;i++){
        std::pair<int,int> v1 = std::make_pair(triangles[t1].v[i],triangles[t1].v[(i+1)%3]);
        for(int j=0;j<3;j++){
            std::pair<int,int> v2 = std::make_pair(triangles[t2].v[j],triangles[t2].v[(j+1)%3]);
            if(v1==v2)return v1;
            v2 = std::make_pair(triangles[t2].v[(j+1)%3],triangles[t2].v[j]);
            if(v1==v2)return v1;
        }
    }
    return std::make_pair(-1,-1);
}

void Triangulation::legalize(int t1, int t2){
    if(t2==-1)return;
    if(t1==-1)return;
    if(!areConnected(t1,t2))return;
    int a[6];
    a[0] = triangles[t1].v[0];
    a[1] = triangles[t1].v[1];
    a[2] = triangles[t1].v[2];
    a[3] = triangles[t2].v[0];
    a[4] = triangles[t2].v[1];
    a[5] = triangles[t2].v[2];
    int b[8];

    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
    for(int i=3;i<6;i++){
        if((a[i]!=b[0]) && (a[i]!=b[1]) && (a[i]!=b[2])) b[3] = a[i];
    }
    b[4] = a[3];
    b[5] = a[4];
    b[6] = a[5];
    for(int i=0;i<3;i++){
        if((a[i]!=b[4]) && (a[i]!=b[5]) && (a[i]!=b[6])) b[7] = a[i];
    }
    if(!isCCW(t1) && !isCCW(t2)){
        std::cout << "ecase" << std::endl;
        {
            __H_BREAKPOINT__;
            if(abs(triangleArea(t1))<AREA_TO_FLIP_EPS || abs(triangleArea(t2))<AREA_TO_FLIP_EPS) return;
            auto vs = getVerticesSharedByTriangles(t1,t2);
            Vertex v1 = vertices[vs.first];
            Vertex v2 = vertices[vs.second];
            vertices[vs.first] = v2;
            vertices[vs.second] = v1;
            __H_BREAKPOINT__;
        }
        return;
    }
    for(int i=0;i<3;i++)if(isInside(t2,vertices[triangles[t1].v[i]].pos)){
        // std::cout << "fcase" << std::endl;
        flipNoChecking(t1,t2);
        // std::cout << "End fcase" << std::endl;
        return;
    }
    for(int i=0;i<3;i++)if(isInside(t1,vertices[triangles[t2].v[i]].pos)){
        // std::cout << "fcase" << std::endl;
        flipNoChecking(t2,t1);
        // std::cout << "End fcase" << std::endl;
        return;
    }
    if(!isCCW(t1) || !isCCW(t2)){
        // std::cout << "Not fcase yet" << std::endl;
        return;
    }
    auto vs = getVerticesSharedByTriangles(t1,t2);
    
    if(inCircle(vertices[b[0]].pos,vertices[b[1]].pos,vertices[b[2]].pos,vertices[b[3]].pos)> IN_CIRCLE_EPS &&
    inCircle(vertices[b[4]].pos,vertices[b[5]].pos,vertices[b[6]].pos,vertices[b[7]].pos)> IN_CIRCLE_EPS &&
    triangleArea(t1)>AREA_TO_FLIP_EPS && triangleArea(t2)>AREA_TO_FLIP_EPS &&
    mod(vertices[vs.first].pos-vertices[vs.second].pos)>EDGE_LENGTH_FLIP_EPS    
    ) {
        // std::cout << "start flip " << t1 << " " << t2 << std::endl << std::flush;
        flip(t1,t2);
        // std::cout << "end flip" << std::endl << std::flush;
    }
}
void Triangulation::addPointInEdge(Vec2 v, int t0, int t1){
#if ASSERT_PROBLEMS
    assert(isCCW(t0)&&isCCW(t1));
    assert(integrity(t0)&&integrity(t1));
    assert(frontTest(t0)&&frontTest(t1));
    assert(areConnected(t0,t1));
    assert(sanity(t0));
    assert(sanity(t1));
    assert(next(t0,t1));
#endif
    remem();
    int p = vcount;
    vertices[vcount++] = Vertex(v);

    int t0_v = -1;
    int t1_v = -1;

    int f0,f3;
    int p1;

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(triangles[t0].t[i]==t1 && triangles[t1].t[j]==t0){
                t0_v = i;
                t1_v = j;
            }
        }
    }

#if ASSERT_PROBLEMS
    assert(t0_v!=-1);
    assert(t1_v!=-1);
    assert(triangles[t0].t[t0_v]==t1);
    assert(triangles[t1].t[t1_v]==t0);
#endif

    f0 = triangles[t0].t[(t0_v+1)%3];
    p1 = triangles[t0].v[(t0_v+2)%3];

    f3 = triangles[t1].t[(t1_v+2)%3];

    int t2 = tcount++;
    int t3 = tcount++;

    triangles[t2] = Triangle(p,p1,triangles[t0].v[t0_v],f0,t0,t3);
    if(f0!=-1)for(int i=0;i<3;i++)if(triangles[f0].t[i]==t0)triangles[f0].t[i] = t2;

    triangles[t3] = Triangle(p,triangles[t1].v[t1_v],p1,f3,t2,t1);
    if(f3!=-1)for(int i=0;i<3;i++)if(triangles[f3].t[i]==t1)triangles[f3].t[i] = t3;

    triangles[t0].v[(t0_v+2)%3] = p;
    triangles[t0].t[(t0_v+1)%3] = t2;
    triangles[t1].v[(t1_v+1)%3] = p;
    triangles[t1].t[(t1_v+2)%3] = t3;

    updateNextToMinOne(t0);
    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
    updateNextToMinOne(t3);
    
    remem();

#if ASSERT_PROBLEMS
    assert(isCCW(t0));
    assert(isCCW(t1));
    assert(isCCW(t2));
    assert(isCCW(t3));
    assert(integrity(t0));
    assert(integrity(t1));
    assert(integrity(t2));
    assert(integrity(t3));
#endif
}

void Triangulation::addPointInEdge(Vec2 v, int t){
#if ASSERT_PROBLEMS
    assert(isCCW(t));
    assert(isInEdge(t,v));
    assert(integrity(t));
#endif
    remem();
    
    int x = triangles[t].t[0] == -1 ? 0 : (triangles[t].t[1] == -1 ? 1 : 2);

    int f1 = triangles[t].t[x];
    int f2 = triangles[t].t[(x+1)%3];

    int p0 = triangles[t].v[(x+2)%3];
    int p1 = triangles[t].v[x];
    
    int t1 = tcount++;
    int p = vcount++;

    vertices[p] = Vertex(v);
    triangles[t1] = Triangle(p0,p1,p,t,f1,f2);
    triangles[t].v[(x+2)%3] = p;
    triangles[t].t[(x+1)%3] = t1;

    if(f2!=-1){
        triangles[f2].t[0] = (triangles[f2].t[0] == t ? t1 : triangles[f2].t[0]);
        triangles[f2].t[1] = (triangles[f2].t[1] == t ? t1 : triangles[f2].t[1]);
        triangles[f2].t[2] = (triangles[f2].t[2] == t ? t1 : triangles[f2].t[2]);
    }
#if ASSERT_PROBLEMS
    assert(isCCW(t)&&isCCW(t1));
    assert(areConnected(t,t1));
    assert(integrity(t));
    assert(integrity(t1));
    assert(integrity(f2));
#endif
    updateNextToMinOne(t);
    updateNextToMinOne(t1);
    updateNextToMinOne(f2);

    remem();
}
bool Triangulation::areConnected(int t1, int t2){ // check if two triangles are neighbours
    int vertcount = 0;
    int facecount = 0;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(triangles[t1].v[i] == triangles[t2].v[j])vertcount++;
            if(triangles[t1].t[i] == t2)facecount++;
            if(triangles[t2].t[j] == t1)facecount++;
        }
    }
    if(!(vertcount==2 && facecount==6))
        return false;
    else return true;
}
void Triangulation::print(){
    for(int i=0;i<tcount;i++){
        std::cout << "Triangle " << i << ":\n";
        std::cout << "P0: " << vertices[triangles[i].v[0]].pos.x << " " << vertices[triangles[i].v[0]].pos.y << std::endl;
        std::cout << "P1: " << vertices[triangles[i].v[1]].pos.x << " " << vertices[triangles[i].v[1]].pos.y << std::endl;
        std::cout << "P2: " << vertices[triangles[i].v[2]].pos.x << " " << vertices[triangles[i].v[2]].pos.y << std::endl;
    }
}
void Triangulation::print_ind(){
    for(int i=0;i<tcount;i++){
        std::cout << "Triangle " << i << ": ";
        std::cout << triangles[i].t[0] << " " << triangles[i].t[1] << " " << triangles[i].t[2] << std::endl;
    }
}
bool Triangulation::isCCW(int f){
    if(f==-1)return true;
    Vec2 p0 = vertices[triangles[f].v[0]].pos;
    Vec2 p1 = vertices[triangles[f].v[1]].pos;
    Vec2 p2 = vertices[triangles[f].v[2]].pos;
    if((crossa(p0,p1)+crossa(p1,p2)+crossa(p2,p0))>IN_TRIANGLE_EPS) return true;
    return false;
}

double Triangulation::triangleArea(int f){
    if(f==-1)return true;
    Vec2 p0 = vertices[triangles[f].v[0]].pos;
    Vec2 p1 = vertices[triangles[f].v[1]].pos;
    Vec2 p2 = vertices[triangles[f].v[2]].pos;
    return (crossa(p0,p1)+crossa(p1,p2)+crossa(p2,p0));
}

void Triangulation::updateNextToMinOne(int t){
    if(t==-1)return;
    for(int i=0;i<3;i++){
        if(triangles[t].t[i]==-1)nextToMinOne=t;
    }
}

void Triangulation::flip(int t1, int t2){
#if ASSERT_PROBLEMS
    assert(sanity(t1)&&sanity(t2));
    assert(areConnected(t1,t2));
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(isCCW(t1)&&isCCW(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif

    Triangle pt1 = triangles[t1];
    Triangle pt2 = triangles[t2];

    int p10,p11,p12,p20,p21,p22;
    int f10,f11,f12,f20,f21,f22;

    p10 = triangles[t1].v[0];
    p11 = triangles[t1].v[1];
    p12 = triangles[t1].v[2];
    f10 = triangles[t1].t[0];
    f11 = triangles[t1].t[1];
    f12 = triangles[t1].t[2];

    p20 = triangles[t2].v[0];
    p21 = triangles[t2].v[1];
    p22 = triangles[t2].v[2];
    f20 = triangles[t2].t[0];
    f21 = triangles[t2].t[1];
    f22 = triangles[t2].t[2];

    //if(f10 == t2)
    if(f11 == t2){ // rotated 1 to the right
        p10 = triangles[t1].v[1];
        p11 = triangles[t1].v[2];
        p12 = triangles[t1].v[0];
        f10 = triangles[t1].t[1];
        f11 = triangles[t1].t[2];
        f12 = triangles[t1].t[0];
    }
    if(f12 == t2){ // rotated 2 to the right
        p10 = triangles[t1].v[2];
        p11 = triangles[t1].v[0];
        p12 = triangles[t1].v[1];
        f10 = triangles[t1].t[2];
        f11 = triangles[t1].t[0];
        f12 = triangles[t1].t[1];
    }
    //if(f20 == t1)
    if(f21 == t1){ 
        p20 = triangles[t2].v[1];
        p21 = triangles[t2].v[2];
        p22 = triangles[t2].v[0];
        f20 = triangles[t2].t[1];
        f21 = triangles[t2].t[2];
        f22 = triangles[t2].t[0];
    }
    if(f22 == t1){ 
        p20 = triangles[t2].v[2];
        p21 = triangles[t2].v[0];
        p22 = triangles[t2].v[1];
        f20 = triangles[t2].t[2];
        f21 = triangles[t2].t[0];
        f22 = triangles[t2].t[1];
    }

    triangles[t1].t[0] = t2;
    triangles[t1].t[1] = f12;
    triangles[t1].t[2] = f21;
    triangles[t1].v[0] = p11; assert(p11==p22);
    triangles[t1].v[1] = p20;
    triangles[t1].v[2] = p10;

    triangles[t2].t[0] = t1;
    triangles[t2].t[1] = f22;
    triangles[t2].t[2] = f11;
    triangles[t2].v[0] = p12; assert(p12==p21);
    triangles[t2].v[1] = p10;
    triangles[t2].v[2] = p20;

    if(!validTriangle(t1) || !validTriangle(t2)){
        if(!validTriangle(t1) && !validTriangle(t2)){
            triangles[t1] = pt1;
            triangles[t2] = pt2;
            for(int i=0;i<3;i++)if(triangles[t1].t[i]==t2){
                int aux = triangles[t1].t[(i+1)%3];
                triangles[t1].t[(i+1)%3] = triangles[t1].t[(i+2)%3];
                triangles[t1].t[(i+2)%3] = aux;
            }
            for(int i=0;i<3;i++)if(triangles[t2].t[i]==t1){
                int aux = triangles[t2].t[(i+1)%3];
                triangles[t2].t[(i+1)%3] = triangles[t2].t[(i+2)%3];
                triangles[t2].t[(i+2)%3] = aux;
            }
#if ASSERT_PROBLEMS
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(isCCW(t1)&&isCCW(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif
            flip(t1,t2);
            return;
        }
        else{
            triangles[t1] = pt1;
            triangles[t2] = pt2;
            return;
        }
    }

    //update f11
    if(f11!=-1)for(int i=0;i<3;i++){
        if(triangles[f11].t[i]==t1)triangles[f11].t[i]=t2;
    }
    //update f21
    if(f21!=-1)for(int i=0;i<3;i++){
        if(triangles[f21].t[i]==t2)triangles[f21].t[i]=t1;
    }

#if ASSERT_PROBLEMS
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(isCCW(t1)&&isCCW(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif

    vertices[p11].tri_index=t1;
    vertices[p20].tri_index=t1;
    vertices[p10].tri_index=t1;

    vertices[p12].tri_index=t2;
    vertices[p10].tri_index=t2;
    vertices[p20].tri_index=t2;

    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
}

void Triangulation::flipNoChecking(int t1, int t2){
#if ASSERT_PROBLEMS
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif

    Triangle pt1 = triangles[t1];
    Triangle pt2 = triangles[t2];

    int p10,p11,p12,p20,p21,p22;
    int f10,f11,f12,f20,f21,f22;

    p10 = triangles[t1].v[0];
    p11 = triangles[t1].v[1];
    p12 = triangles[t1].v[2];
    f10 = triangles[t1].t[0];
    f11 = triangles[t1].t[1];
    f12 = triangles[t1].t[2];

    p20 = triangles[t2].v[0];
    p21 = triangles[t2].v[1];
    p22 = triangles[t2].v[2];
    f20 = triangles[t2].t[0];
    f21 = triangles[t2].t[1];
    f22 = triangles[t2].t[2];

    //if(f10 == t2)
    if(f11 == t2){ // rotated 1 to the right
        p10 = triangles[t1].v[1];
        p11 = triangles[t1].v[2];
        p12 = triangles[t1].v[0];
        f10 = triangles[t1].t[1];
        f11 = triangles[t1].t[2];
        f12 = triangles[t1].t[0];
    }
    if(f12 == t2){ // rotated 2 to the right
        p10 = triangles[t1].v[2];
        p11 = triangles[t1].v[0];
        p12 = triangles[t1].v[1];
        f10 = triangles[t1].t[2];
        f11 = triangles[t1].t[0];
        f12 = triangles[t1].t[1];
    }
    //if(f20 == t1)
    if(f21 == t1){ 
        p20 = triangles[t2].v[1];
        p21 = triangles[t2].v[2];
        p22 = triangles[t2].v[0];
        f20 = triangles[t2].t[1];
        f21 = triangles[t2].t[2];
        f22 = triangles[t2].t[0];
    }
    if(f22 == t1){ 
        p20 = triangles[t2].v[2];
        p21 = triangles[t2].v[0];
        p22 = triangles[t2].v[1];
        f20 = triangles[t2].t[2];
        f21 = triangles[t2].t[0];
        f22 = triangles[t2].t[1];
    }

    assert(f20==t1 && f10==t2);

    triangles[t1].t[0] = t2;
    triangles[t1].t[1] = f12;
    triangles[t1].t[2] = f21;
    triangles[t1].v[0] = p11; assert(p11==p22);
    triangles[t1].v[1] = p20;
    triangles[t1].v[2] = p10;
    
    triangles[t2].t[0] = t1;
    triangles[t2].t[1] = f22;
    triangles[t2].t[2] = f11;
    triangles[t2].v[0] = p12; assert(p12==p21);
    triangles[t2].v[1] = p10;
    triangles[t2].v[2] = p20;

    if(!validTriangle(t1) || !validTriangle(t2)){
        if(!validTriangle(t1) && !validTriangle(t2)){
            triangles[t1] = pt1;
            triangles[t2] = pt2;
            for(int i=0;i<3;i++)if(triangles[t1].t[i]==t2){
                int aux = triangles[t1].t[(i+1)%3];
                triangles[t1].t[(i+1)%3] = triangles[t1].t[(i+2)%3];
                triangles[t1].t[(i+2)%3] = aux;
            }
            for(int i=0;i<3;i++)if(triangles[t2].t[i]==t1){
                int aux = triangles[t2].t[(i+1)%3];
                triangles[t2].t[(i+1)%3] = triangles[t2].t[(i+2)%3];
                triangles[t2].t[(i+2)%3] = aux;
            }
#if ASSERT_PROBLEMS
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(isCCW(t1)&&isCCW(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif
            flip(t1,t2);
            return;
        }
        else{
            triangles[t1] = pt1;
            triangles[t2] = pt2;
            return;
        }
    }

    //update f11
    if(f11!=-1)for(int i=0;i<3;i++){
        if(triangles[f11].t[i]==t1)triangles[f11].t[i]=t2;
    }
    //update f21
    if(f21!=-1)for(int i=0;i<3;i++){
        if(triangles[f21].t[i]==t2)triangles[f21].t[i]=t1;
    }

#if ASSERT_PROBLEMS
    assert(validTriangle(t1)&&validTriangle(t2));
    assert(isCCW(t1));
    assert(isCCW(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(frontTest(t1));
    assert(frontTest(t2));
    assert(frontTest(f11));
    assert(frontTest(f12));
    assert(frontTest(f21));
    assert(frontTest(f22));
#endif

    vertices[p11].tri_index=t1;
    vertices[p20].tri_index=t1;
    vertices[p10].tri_index=t1;

    vertices[p12].tri_index=t2;
    vertices[p10].tri_index=t2;
    vertices[p20].tri_index=t2;

    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
}

Triangulation::~Triangulation(){
    delete[] triangles;
    delete[] vertices;
}

bool Triangulation::sanity(int t){
    if(t==-1)return true;
    for(int i=0;i<3;i++){
        int count = 0;
        int f = triangles[t].t[i];
        if(f==-1)continue;
        for(int j=0;j<3;j++){
            for(int k=0;k<3;k++){
                if(triangles[t].v[k]==triangles[f].v[j])count++;
            }
        }
        if(count != 2) return false;
    }
    return true;
}

// -------------------------- T2 ------------------------------

int Triangulation::calcLongestEdge(int t){
    int longestEdge = 0;
    double longestEdgeSqrtLength = sqrtLength(vertices[triangles[t].v[1]].pos-vertices[triangles[t].v[2]].pos);
    for(int i=1;i<3;i++){
        double length = sqrtLength(vertices[triangles[t].v[(i+1)%3]].pos-vertices[triangles[t].v[(i+2)%3]].pos);
        if(length>longestEdgeSqrtLength){
            longestEdgeSqrtLength = length;
            longestEdge = i;
        }
    }
    return longestEdge;
}

std::vector<int> Triangulation::calcLepp(int t){
    std::vector<int> res;
    int currt = t;
    res.push_back(currt);
    while(true){
        int currt = triangles[res[res.size()-1]].t[calcLongestEdge(res[res.size()-1])];
        res.push_back(currt);
        if(res[res.size()-1] == -1) break;
        if((res.size()>2) && (res[res.size()-1] == res[res.size()-3])){
            res.pop_back();
            break;
        }
    }
    return res;
}

void Triangulation::centroidAll(double angle){
    bool global_do = true;
    while(global_do){
        global_do = false;
        for(int i=0;i<tcount;i++){
            bool flag_do = true;
            while(flag_do){
                flag_do = false;
                for(int j=0;j<3;j++){
                    Vec2 x = vertices[triangles[i].v[(j+1)%3]].pos;
                    Vec2 y = vertices[triangles[i].v[(j+2)%3]].pos;
                    Vec2 z = vertices[triangles[i].v[j]].pos;

                    Vec2 a = y-z;
                    Vec2 b = x-z;

                    double s_angle = std::acos(dot(a,b)/(mod(a)*mod(b))) * (180.0 / ID_PI);
                    if(s_angle <=  angle){
                        flag_do = true;
                        global_do = true;
                    }
                }
                
                if(!flag_do) continue;

                std::vector<int> le = calcLepp(i);
                int f1 = le[le.size()-1],f2 = le[le.size()-2];

                if(f1==-1 && f2==-1) continue;
                if(f1==-1){
                    longestEdgeBisect(f2);
                    continue;
                }
                if(f2==-1){
                    longestEdgeBisect(f1);
                    continue;
                }

                int points[4];
                points[0] = triangles[f1].v[0];
                points[1] = triangles[f1].v[1];
                points[2] = triangles[f1].v[2];
                points[3] = -1;
                for(int j=0;j<3;j++){
                    int p = triangles[f2].v[j];
                    bool isDiff = true;
                    for(int k=0;k<3;k++){
                        if(points[k]==p)isDiff=false;
                    }
                    if(isDiff)points[3] = p;
                }

                //if(points[3]==-1)continue;

                Vec2 p = (vertices[points[0]].pos+vertices[points[1]].pos+vertices[points[2]].pos+vertices[points[3]].pos)/4;

                delaunayInsertion(p);
            }
        }
    }
}

void Triangulation::addCentroids(){
    int actTcount = tcount;
    for(int i=0;i<actTcount;i++){
        Vec2 p = (vertices[triangles[i].v[0]].pos + vertices[triangles[i].v[1]].pos + vertices[triangles[i].v[2]].pos)/3;
        delaunayInsertion(p);
    }
}

void Triangulation::longestEdgeBisect(int t){
    int op1 = -1;
    for(int i=0;i<3;i++){
        if(triangles[t].t[i]==-1) op1 = i;
    }
    Vec2 p = (vertices[triangles[t].v[(op1+1)%3]].pos + vertices[triangles[t].v[(op1+2)%3]].pos)/2;
    addPointInEdge(p,t);
}

void Triangulation::remem(){
    if(tcount >= maxTriangles-4){ // we must get more space
        // std::cout << "al triangulos" << std::endl;
        Triangle *newTriangles = new Triangle[maxTriangles*2];
        std::copy(triangles,triangles+tcount,newTriangles);
        delete[] triangles;
        triangles = newTriangles;
        maxTriangles *= 2;
    }

    if(vcount >= maxVertices-3){ // we must get more space
        // std::cout << "al vertices" << std::endl;
        Vertex *newVertices = new Vertex[maxVertices*2];
        std::copy(vertices,vertices+vcount,newVertices);
        delete[] vertices;
        vertices = newVertices;
        maxVertices *= 2;
    }
}

bool Triangulation::next(int t0, int t1){
    if(t0==-1 && t1==-1)return true;
    if(t0==-1){
        for(int i=0;i<3;i++){
            if(triangles[t1].t[i]==-1)return true;
        }
        return false;
    }
    if(t1==-1){
        for(int i=0;i<3;i++){
            if(triangles[t0].t[i]==-1)return true;
        }
        return false;
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(triangles[t0].t[i]==t1 && triangles[t1].t[j]==t0)return true;
        }
    }
    return false;
}

void save_mesh(Triangulation *t, const char *filename){
	int vcount = t->vcount;
	int fcount = t->tcount; 
	int ecount = 0;
	//following line is for computer with other languages.
	setlocale(LC_NUMERIC, "POSIX");
	FILE *file_descriptor = fopen(filename,"w");
	fprintf(file_descriptor,"OFF\n");
	fprintf(file_descriptor,"%d %d %d\n",vcount, fcount, ecount);
	for(int i=0; i<vcount; i++) {
		fprintf(file_descriptor,"%f %f %f\n",t->vertices[i].pos.x,t->vertices[i].pos.y,0.0);
	}
	for(int i=0; i<fcount; i++) {
		fprintf(file_descriptor,"%d %d %d %d\n", 3, t->triangles[i].v[0],t->triangles[i].v[1],t->triangles[i].v[2]);
	}
	fclose(file_descriptor);
    setlocale(LC_NUMERIC, "");
}

void Triangulation::whichTriangle(){
    for(int i=0;i<vcount;i++){
        for(int j=0;j<tcount;j++){
            for(int k=0;k<3;k++){
                if(triangles[j].v[k]==i)vertices[i].tri_index=j;
            }
        }
    }
}

std::set<int> Triangulation::getNeighbours(int index){
    int tri_index = vertices[index].tri_index;
    std::set<int> neighbours = std::set<int>();
    std::set<int> trianglesChecked = std::set<int>();
    std::vector<int> checkingTriangles = std::vector<int>();
    checkingTriangles.push_back(tri_index);
    while(!checkingTriangles.empty()){
        int curr_triangle = checkingTriangles[checkingTriangles.size()-1];
        checkingTriangles.pop_back();
        bool isInside = false;
        for(int i=0;i<3;i++){
            if(triangles[curr_triangle].v[i]==index){isInside=true;break;}
        }
        if(!isInside) continue;
        for(int i=0;i<3;i++){
            if(triangles[curr_triangle].v[i]==index){continue;}
            else neighbours.insert(triangles[curr_triangle].v[i]);
        }
        for(int i=0;i<3;i++){
            if(trianglesChecked.find(triangles[curr_triangle].t[i])==trianglesChecked.end())
                if(triangles[curr_triangle].t[i]!=-1)checkingTriangles.push_back(triangles[curr_triangle].t[i]);
        }
        trianglesChecked.insert(curr_triangle);
    }
    return neighbours;
}

std::set<int> Triangulation::getNeighbourTriangles(int index){
    int tri_index = vertices[index].tri_index;
    std::set<int> trianglesChecked = std::set<int>();
    std::vector<int> checkingTriangles = std::vector<int>();
    checkingTriangles.push_back(tri_index);
    while(checkingTriangles.size()>0){
        int curr_triangle = checkingTriangles.back();
        checkingTriangles.pop_back();
        if(curr_triangle==-1)continue;
        trianglesChecked.insert(curr_triangle);
        bool isInside = false;
        for(int i=0;i<3;i++){
            if(triangles[curr_triangle].v[i]==index){isInside=true;break;}
        }
        
        if(!isInside) continue;
        for(int i=0;i<3;i++){
            if(triangles[curr_triangle].v[i]==index){continue;}
        }
        for(int i=0;i<3;i++){
            if(trianglesChecked.find(triangles[curr_triangle].t[i])==trianglesChecked.end())
                if(triangles[curr_triangle].t[i]!=-1)
                    checkingTriangles.push_back(triangles[curr_triangle].t[i]);
        }
        
    }
    return trianglesChecked;
}

float Triangulation::closestNeighbourDistance(int index){
    std::set<int> n = getNeighbours(index);
    int closestNeighbour = -1;
    float closestDistance = abs(p1[0]-p0[0]) * abs(p2[1]-p0[1]);
    for(int v: n){
        float dist = mod(vertices[index].pos-vertices[v].pos);
        if(dist<closestDistance){
            closestNeighbour = v;
            closestDistance = dist;
        }
    }
    return closestDistance;
}

void Triangulation::movePoint(int index, Vec2 delta){
    point_being_moved = index;
    std::set<int> nt = getNeighbourTriangles(index);
    if(closestNeighbourDistance(index)<2*radius){
        velocity[index]*=-1;
        vertices[index].pos+=delta*-1;
        // std::cout << "BOUNCE " << std::endl;
    }
    else vertices[index].pos+=delta;
    for(int t: nt){
        for(int i=0;i<3;i++){
            legalize(t,triangles[t].t[i]);
        }
    }
}

Triangulation::RemovedVertex Triangulation::removeVertex(int v){
    Vertex vx = vertices[v];
    std::set<int> nt = getNeighbourTriangles(v);
    while(nt.size()>3){
        int t1 = *nt.begin();
        nt.erase(nt.begin());
        if(t1==-1)continue;
        int t2 = -1;
        if(nt.find(triangles[t1].t[0])!=nt.end()) t2 = triangles[t1].t[0];
        else if(nt.find(triangles[t1].t[1])!=nt.end()) t2 = triangles[t1].t[1];
        else if(nt.find(triangles[t1].t[2])!=nt.end()) t2 = triangles[t1].t[2];
        //if(t2==-1)continue;
        flip(t1,t2);
        nt = getNeighbourTriangles(v);
    }
    auto it = nt.begin();
    RemovedVertex res{{*it++,*it++,*it},v,vx};
    return res;
}

void Triangulation::reAddVertex(int v, Vertex vx){

}

std::set<int> Triangulation::getFRNN(int index, float r){
    int tri_index = vertices[index].tri_index;
    std::set<int> neighbours = std::set<int>();
    std::set<int> trianglesChecked = std::set<int>();
    std::vector<int> checkingTriangles = std::vector<int>();
    checkingTriangles.push_back(tri_index);
    while(!checkingTriangles.empty()){
        int curr_triangle = checkingTriangles[checkingTriangles.size()-1];
        checkingTriangles.pop_back();
        bool isInside = false;
        for(int i=0;i<3;i++){
            if(triangles[curr_triangle].v[i]==index){continue;}
            else if(dist2(vertices[triangles[curr_triangle].v[i]].pos,vertices[index].pos)<=r){
                neighbours.insert(triangles[curr_triangle].v[i]);
                isInside=true;
            }
        }
        for(int i=0;i<3;i++){
            if(trianglesChecked.find(triangles[curr_triangle].t[i])==trianglesChecked.end())
                if(triangles[curr_triangle].t[i]!=-1 && isInside)checkingTriangles.push_back(triangles[curr_triangle].t[i]);
        }
        trianglesChecked.insert(curr_triangle);
    }
    return neighbours;
}