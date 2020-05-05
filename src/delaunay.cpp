#include <iostream>

#include "delaunay.h"
#include "constants.h"

//HELPER FUNCTIONS
double crossa(Vec2 a, Vec2 b){
    return a.x*b.y-a.y*b.x;
}

bool isLeft(Vec2 a, Vec2 b){
    return crossa(a,b) > IN_TRIANGLE_EPS;
}

bool isRight(Vec2 a, Vec2 b){
    return crossa(a,b) < IN_TRIANGLE_EPS;
}

bool mightBeLeft(Vec2 a, Vec2 b){
    return crossa(a,b) >= IN_TRIANGLE_EPS;
}

template<class T>
T amin(T a, T b){
    if(a<b)return a;
    return b;
}

double det(double a, double b, double c, double d, double e, double f, double g, double h, double i){
    return a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
}

double inCircle(Vec2 a, Vec2 b, Vec2 c, Vec2 d){
    return det( a.x-d.x,a.y-d.y,(a.x-d.x)*(a.x-d.x)+(a.y-d.y)*(a.y-d.y),
                b.x-d.x,b.y-d.y,(b.x-d.x)*(b.x-d.x)+(b.y-d.y)*(b.y-d.y),
                c.x-d.x,c.y-d.y,(c.x-d.x)*(c.x-d.x)+(c.y-d.y)*(c.y-d.y));
}

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
    Vec2 p1 = vertices[triangles[t].v[0]].pos;
    Vec2 p2 = vertices[triangles[t].v[1]].pos;
    Vec2 p3 = vertices[triangles[t].v[2]].pos;
    // b-a goes from a to b
    if(isLeft(p2-p1,p-p1) && isLeft(p3-p2,p-p2) && isLeft(p1-p3,p-p3)) return true;
    return false;
}

bool Triangulation::isInEdge(int t, Vec2 p){
    if(isInside(t,p))return false; // in our context this will never happend, so we can skip this check
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

Triangulation::Triangulation(std::vector<Vec2> points, int numP) {
    Vec2 p0 = Vec2(-0.9,-0.9);
    Vec2 p1 = Vec2(0.9,-0.9);
    Vec2 p2 = Vec2(0.9,0.9);
    Vec2 p3 = Vec2(-0.9,0.9);
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
        addPoint(points[i]);
    }
}

Triangulation::Triangulation(std::vector<Vec2> points, int numP, Vec2 p0, Vec2 p1, Vec2 p2) {
    maxTriangles = numP*2+1;
    maxVertices = numP+3;
    vertices = new Vertex[numP+3]; // num of vertices
    triangles = new Triangle[numP*2+1]; // 2(n+3) - 2 - 3 = 2n+1 // num of faces

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
    //std::cout << "Insert inside: " << tri_index << std::endl;

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

double dist2(Vec2 a,Vec2 b){
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

int Triangulation::findContainerTriangleLogSearch(Vec2 p, Vec2 initialPoint, int prop, int cameFrom){
    if(prop == -1) return -1;
    if(isInside(prop,p))return prop;
    Vec2 v = initialPoint;
    for(int i=0;i<3;i++){
        int f = triangles[prop].t[i];
        if(f==-1)continue;
        Vec2 a = vertices[triangles[prop].v[(i+1)%3]].pos;
        Vec2 b = vertices[triangles[prop].v[(i+2)%3]].pos;
        if(
            (mightBeLeft(p-v,a-v) && mightBeLeft(b-v,p-v)) ||
            (mightBeLeft(p-v,b-v) && mightBeLeft(a-v,p-v))
        ){
            if(f!=cameFrom)return findContainerTriangleLogSearch(p,v,f,prop);
        }
    }
    
    return -1;
}

void Triangulation::addPoint(Vec2 p){
#if ASSERT_PROBLEMS
    assert(
        triangles[nextToMinOne].t[0]==-1 ||
        triangles[nextToMinOne].t[1]==-1 ||
        triangles[nextToMinOne].t[2]==-1
    );
#endif
    Vec2 initialPoint = (vertices[triangles[nextToMinOne].v[0]].pos+vertices[triangles[nextToMinOne].v[1]].pos+vertices[triangles[nextToMinOne].v[2]].pos)/3;
    int tri_index = findContainerTriangleLogSearch(p,initialPoint,nextToMinOne,-1);
    //int tri_index = findContainerTriangleLinearSearch(p);
    if(tri_index!=-1){
        this->incount++;
        addPointInside(p,tri_index);
        int a = tri_index,b=tcount-1,c=tcount-2;
        int aa = -1;
        int bb = -1;
        int cc = -1;
        for(int i=0;i<3;i++){
            if(triangles[a].t[i]!=b && triangles[a].t[i]!=c)aa=triangles[a].t[i];
        }
        for(int i=0;i<3;i++){
            if(triangles[b].t[i]!=a && triangles[b].t[i]!=c)bb=triangles[b].t[i];
        }
        for(int i=0;i<3;i++){
            if(triangles[c].t[i]!=a && triangles[c].t[i]!=b)cc=triangles[c].t[i];
        }
        legalize(a,aa);
        legalize(b,bb);
        legalize(c,cc);
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
        //addPointInEdge(p,t1,t2);
        //legalize(t1,t2);
        //legalize(t1,tcount-2);
        //legalize(tcount-1,tcount-2);
        //legalize(t2,tcount-1);
        return;
    }
    else if((t1!=-1) && (t2==-1)){
        this->oedgecount++;
        //addPointInEdge(p,t1);
        //int aa=-1,bb=-1;
        //for(int i=0;i<3;i++){
        //    if(triangles[t1].t[i]!=-1 && triangles[t1].t[i]!=(tcount-1))aa=triangles[t1].t[i];
        //}
        //for(int i=0;i<3;i++){
        //    if(triangles[(tcount-1)].t[i]!=-1 && triangles[(tcount-1)].t[i]!=t1)bb=triangles[(tcount-1)].t[i];
        //}
        //legalize(t1,aa);
        //legalize(tcount-1,bb);
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
    if(inCircle(vertices[b[0]].pos,vertices[b[1]].pos,vertices[b[2]].pos,vertices[b[3]].pos)>-IN_CIRCLE_EPS &&
    inCircle(vertices[b[4]].pos,vertices[b[5]].pos,vertices[b[6]].pos,vertices[b[7]].pos)>-IN_CIRCLE_EPS) {
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

    //std::cout << t << ": " << triangles[t].t[0] << " " << triangles[t].t[1] << " " << triangles[t].t[2] << std::endl;

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
    //std::cout << t << ": " << triangles[t].t[0] << " " << triangles[t].t[1] << " " << triangles[t].t[2] << std::endl;
    //std::cout << t1 << ": " << triangles[t1].t[0] << " " << triangles[t1].t[1] << " " << triangles[t1].t[2] << std::endl;
    //std::cout << t << ": " << triangles[t].v[0] << " " << triangles[t].v[1] << " " << triangles[t].v[2] << std::endl;
    //std::cout << t1 << ": " << triangles[t1].v[0] << " " << triangles[t1].v[1] << " " << triangles[t1].v[2] << std::endl;
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
    //std::cout << crossa(p0,p1) << " + " << crossa(p1,p2) << " + " << crossa(p2,p0) << "= " << crossa(p0,p1)+crossa(p1,p2)+crossa(p2,p0) << std::endl;
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
    //std::cout << "Flip between: " << t1 << " " << t2 << std::endl;

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

    //std::cout << p11 << " " << p22 << std::endl;
    //std::cout << p12 << " " << p21 << std::endl;

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
    //std::cout << "flip done" << std::endl;
    updateNextToMinOne(t1);
    updateNextToMinOne(t2);
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