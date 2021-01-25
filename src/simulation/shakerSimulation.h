#include "delaunay.h"

#define REAL double

inline Vec2 operator*(Vec2 v, REAL d){
    return Vec2(v.x*d,v.y*d);
}

class TriangulationSaker {
public:
    Triangulation* t;
    int numP;
    Vec2* velocity;
    REAL maxVel;
    TriangulationSaker(Triangulation *t, REAL maxVel) : t(t), numP(t->vcount), maxVel(maxVel) {
        velocity = new Vec2[t->maxVertices];
        t->velocity = this->velocity;
        initRandomVel();
    }
    ~TriangulationSaker(){
        delete velocity;
    }
    void step(REAL dt){
        for(int i=0;i<t->vcount;i++){
            if(t->vertices[i].pos[0] >= (t->p1[0]-velocity[i][0]) || t->vertices[i].pos[0] <= (t->p0[0]-velocity[i][0])) velocity[i][0]*=-1;
            if(t->vertices[i].pos[1] >= (t->p2[1]-velocity[i][1]) || t->vertices[i].pos[1] <= (t->p0[1]-velocity[i][1])) velocity[i][1]*=-1;
            Vec2 d = velocity[i]*dt;
            t->movePoint(i,d);
            t->getFRNN_distance(i,5); // This doesnt take too much time
        }
    }
    void initRandomVel(){
        for(int i=0;i<4;i++){
            velocity[i] = Vec2{0,0};
        }
        for(int i=4;i<numP;i++){
            velocity[i] = Vec2{maxVel*((REAL)i/numP),maxVel*((REAL)i/numP)};
        }
    }
};