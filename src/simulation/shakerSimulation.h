#include "delaunay.h"


class TriangulationSaker {
public:
    Triangulation* t;
    int numP;
    Vec2* velocity;
    float maxVel;
    TriangulationSaker(Triangulation *t, float maxVel) : t(t), numP(t->vcount), maxVel(maxVel) {
        velocity = new Vec2[t->maxVertices];
        t->velocity = this->velocity;
    }
    ~TriangulationSaker(){
        delete velocity;
    }
    void shakeTriangulation(float dt){
        for(int i=0;i<t->vcount;i++){
            Vec2 d = velocity[i]*dt;
            t->movePoint(i,d);
            if(t->vertices[i].pos[0] >= (t->p1[0]-velocity[i][0]) || t->vertices[i].pos[0] <= (t->p0[0]-velocity[i][0])) velocity[i][0]*=-1;
            if(t->vertices[i].pos[1] >= (t->p2[1]-velocity[i][1]) || t->vertices[i].pos[1] <= (t->p0[1]-velocity[i][1])) velocity[i][1]*=-1;
        }
    }
    void initRandomVel(){
        for(int i=0;i<4;i++){
            velocity[i] = Vec2{0,0};
        }
        for(int i=4;i<numP;i++){
            velocity[i] = Vec2{maxVel*((float)i/numP),maxVel*((float)i/numP)};
        }
    }
};