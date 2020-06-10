#include <iostream>

#include "delaunay.h"
#include "constants.h"

#include <queue>

#include "utils.h"

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
    vertices = new Vertex[numP+6]; // num of vertices
    triangles = new Triangle[numP*2+7]; // 2(n+6) - 2 - 3 = 2n+7 // num of faces

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

    for(int i=0;i<points.size();i++){
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

    return (a&&b)&&c;
}

bool Triangulation::frontTest(int t){ // checks that every point is in the same index of a triangle that the triangle in front of it
    bool res = true;
    for(int i=0;i<3;i++){
        int v = triangles[t].v[i];
        int f = triangles[t].t[i];
        if(f!=-1)for(int j=0;j<3;j++){
            if(triangles[f].v[j]==v)res=false;
        }
    }
    return res;
}
void Triangulation::addPointInside(Vec2 v, int tri_index){
    int f = tri_index;
    int f1 = tcount++;
    int f2 = tcount++;

    int p = vcount++;

    int p0 = triangles[f].v[0];
    int p1 = triangles[f].v[1];
    int p2 = triangles[f].v[2];
    int t0 = triangles[f].t[0];
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

Vec2 operator/(Vec2 v, float a){
    return Vec2(v.x/a,v.y/a);
}

Vec2 operator*(Vec2 v, float a){
    return Vec2(v.x*a,v.y*a);
}

double dist2(Vec2 a,Vec2 b){
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
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

    return -1;
}

void Triangulation::delaunayInsertion(Vec2 p){
#if ASSERT_PROBLEMS
    assert(
        triangles[nextToMinOne].t[0]==-1 ||
        triangles[nextToMinOne].t[1]==-1 ||
        triangles[nextToMinOne].t[2]==-1
    );
#endif
    Vec2 initialPoint = (vertices[triangles[nextToMinOne].v[0]].pos+vertices[triangles[nextToMinOne].v[1]].pos+vertices[triangles[nextToMinOne].v[2]].pos)/3;
    int tri_index;
    if(doLogSearch) tri_index = findContainerTriangleSqrtSearch(p,initialPoint,nextToMinOne,-1);
    else tri_index = findContainerTriangleLinearSearch(p);
    if(tri_index!=-1){
        this->incount++;
        addPointInside(p,tri_index);
        int a = tri_index,b=tcount-1,c=tcount-2;
        for (int i = 0; i < 3; i++) {
            legalize(a, triangles[a].t[i]);
            legalize(b, triangles[b].t[i]);
            legalize(c, triangles[c].t[i]);
        }
        return;
    }
}

void Triangulation::legalize(int t1, int t2){
    if(t2==-1)return;
    if(t1==-1)return;
    int a[6];
    a[0] = triangles[t1].v[0];
    a[1] = triangles[t1].v[1];
    a[2] = triangles[t1].v[2];
    a[3] = triangles[t2].v[0];
    a[4] = triangles[t2].v[1];
    a[5] = triangles[t2].v[2];
    int b[8];
    int count = 0;

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
    if(inCircle(vertices[b[0]].pos,vertices[b[1]].pos,vertices[b[2]].pos,vertices[b[3]].pos)>0.000001 &&
    inCircle(vertices[b[4]].pos,vertices[b[5]].pos,vertices[b[6]].pos,vertices[b[7]].pos)>0.00000001) {
        flip(t1,t2);
    }
}
void Triangulation::addPointInEdge(Vec2 v, int t1, int t2){
#if ASSERT_PROBLEMS
    assert(isCCW(t1)&&isCCW(t2));
    assert(isInEdge(t1,v)&&isInEdge(t2,v));
    assert(integrity(t1)&&integrity(t2));
#endif
    int f10,f20,f11,f21,f12,f22,p10,p20,p11,p21,p12,p22;
    
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

    int t3 = tcount++;
    int t4 = tcount++;
    int p = vcount++;
    vertices[p] = Vertex(v);

    if(f10 == t2){
        triangles[t3] = Triangle(p,p12,p10,f11,t1,t4);
        triangles[t1].v[2] = p;
        triangles[t1].t[1] = t3;
    }
    else if(f11 == t2){
        triangles[t3] = Triangle(p,p10,p11,f12,t1,t4);
        triangles[t1].v[0] = p;
        triangles[t1].t[2] = t3;
    }
    else if(f12 == t2){
        triangles[t3] = Triangle(p,p11,p12,f10,t1,t4);
        triangles[t1].v[1] = p;
        triangles[t1].t[0] = t3;
    }
    if(f20 == t1){
        triangles[t4] = Triangle(p,p20,p21,f22,t3,t2);
        triangles[t2].v[1] = p;
        triangles[t2].t[2] = t4;
    }
    else if(f21 == t1){
        triangles[t4] = Triangle(p,p21,p22,f20,t3,t2);
        triangles[t2].v[2] = p;
        triangles[t2].t[0] = t4;
    }
    else if(f22 == t1){
        triangles[t4] = Triangle(p,p22,p20,f21,t3,t2);
        triangles[t2].v[0] = p;
        triangles[t2].t[1] = t4;
    }
}
void Triangulation::addPointInEdge(Vec2 v, int t){
#if ASSERT_PROBLEMS
    assert(isCCW(t));
    assert(isInEdge(t,v));
    assert(integrity(t));
#endif

    int x = triangles[t].t[0] == -1 ? 0 : (triangles[t].t[1] == -1 ? 1 : 2);

    int f0 = triangles[t].t[(x+2)%3];
    int f1 = triangles[t].t[x];
    int f2 = triangles[t].t[(x+1)%3];

    int p0 = triangles[t].v[(x+2)%3];
    int p1 = triangles[t].v[x];
    int p2 = triangles[t].v[(x+1)%3];
    
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
}
bool Triangulation::areConnected(int t1, int t2){
    int vertcount = 0;
    int facecount = 0;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(triangles[t1].v[i] == triangles[t2].v[j])vertcount++;
            if(triangles[t1].t[i] == t2)facecount++;
            if(triangles[t2].t[j] == t1)facecount++;
        }
    }
    return vertcount==2 && facecount==6;
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
    if((crossa(p0,p1)+crossa(p1,p2)+crossa(p2,p0))>-IN_TRIANGLE_EPS) return true;
    
    return false;
}

void Triangulation::updateNextToMinOne(int t){
    if(t==-1)return;
    for(int i=0;i<3;i++){
        if(triangles[t].t[i]==-1)nextToMinOne=t;
    }
}

void Triangulation::flip(int t1, int t2){
#if ASSERT_PROBLEMS
    assert(isCCW(t1)&&isCCW(t2));
    assert(integrity(t1)&&integrity(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif

    int p10,p11,p12,p20,p21,p22;
    int f10,f11,f12,f20,f21,f22;

    int off = 0;

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

    //update f11
    if(f11!=-1)for(int i=0;i<3;i++){
        if(triangles[f11].t[i]==t1)triangles[f11].t[i]=t2;
    }
    //update f21
    if(f21!=-1)for(int i=0;i<3;i++){
        if(triangles[f21].t[i]==t2)triangles[f21].t[i]=t1;
    }

#if ASSERT_PROBLEMS
    assert(integrity(t1)&&integrity(t2));
    assert(isCCW(t1)&&isCCW(t2));
    assert(frontTest(t1)&&frontTest(t2));
#endif
    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
    legalize(t2,f20);
    legalize(t2,f22);
    legalize(t1,f10);
    legalize(t1,f12);
}

Triangulation::~Triangulation(){
    delete triangles;
    delete vertices;
}

bool Triangulation::sanity(int t){
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
    do{
        int currt = triangles[currt].t[calcLongestEdge(currt)];
        res.push_back(currt);
        std::cout << currt << std::endl;
        if(currt == -1) break;
        if((res.size()>3) && (res[res.size()-1] == res[res.size()-3])) break;
    } while(
        (calcLongestEdge(res[res.size()-1])!=calcLongestEdge(res[res.size()-2]))
    );
    return res;
}

