#pragma once

#include "delaunay.h"

#define REAL double

template<class T>
T pow2(T a){
    return a*a;
}

template<class T>
T pow3(T a){
    return a*a*a;
}

inline REAL hmod(Vec2 a){
    return sqrt(a[0]*a[0] + a[1]*a[1]);
}

inline Vec2 operator*(Vec2 v, REAL d){
    return Vec2(v.x*d,v.y*d);
}

class BoidsSimulation {
public:
    Triangulation* t;
    int numP;
    Vec2* velocity;
    REAL maxVel;

    REAL dt; // timestep of the simulation
    REAL g; // gravity

    REAL h; // interaction radius
    REAL p0; // rest density
    REAL k; // stiffness parameter
    REAL* P; // pseudo pressure
    REAL* p; // each particles density sum(pow2((1-rij/h)))

    REAL* p_near; // near density sum(pow3(1-rij/h)))
    REAL k_near; // near density stiffness factor
    REAL* P_near; // near pseudo pressure
    Vec2* dx;
    float e = 0.08;

    BoidsSimulation(Triangulation *t, REAL maxVel) : t(t), numP(t->vcount), maxVel(maxVel) {
        velocity = new Vec2[t->maxVertices];
        t->velocity = this->velocity;
        this->numP = t->vcount;

        for(int i=0;i<numP;i++){
            velocity[i] = Vec2{0,0};
        }
    }

    ~BoidsSimulation(){
        delete velocity;
    }

    void step(REAL dt){

        for(int i=0;i<numP;i++) {
            std::set<int> neighbours = t->getFRNN(i,h);
            Vec2 force{0,0};
            Vec2 selfPos{t->vertices[i].pos.x,t->vertices[i].pos.y};

            //count neighbours
            int numNeighs = 0;
            for(auto n: neighbours){
                Vec2 d{t->vertices[n].pos.x - selfPos.x,t->vertices[n].pos.y - selfPos.y};
                if (hmod(d) < e) {
                    numNeighs++;
                }
            }

            //separation
            for(auto n: neighbours){
                Vec2 d{t->vertices[n].pos.x - selfPos.x,t->vertices[n].pos.y - selfPos.y};
                force -= d*((float)(hmod(d)<e));
            }

            //cohesion
            Vec2 center{0.0, 0.0};

            for (auto n : neighbours) {
                Vec2 d{t->vertices[n].pos.x - selfPos.x,t->vertices[n].pos.y - selfPos.y};
                center -= t->vertices[n].pos*(float)(hmod(d) < e);
            }
            if(numNeighs>0)force += (center - selfPos)/(double)numNeighs;

            //alignement
            for (auto n : neighbours) {
                Vec2 d{t->vertices[n].pos.x - selfPos.x,t->vertices[n].pos.y - selfPos.y};
                force += (velocity[n]/(double)numNeighs)*(float)(hmod(d) < e);
            }

        }

        for(int i=0;i<numP;i++)
        t->movePoint(i,velocity[i]);

        // for(int i=0;i<t->vcount;i++){
        //     if(t->vertices[i].pos[0] >= (t->p1[0]-velocity[i][0]) || t->vertices[i].pos[0] <= (t->p0[0]-velocity[i][0])) velocity[i][0]*=-1;
        //     if(t->vertices[i].pos[1] >= (t->p2[1]-velocity[i][1]) || t->vertices[i].pos[1] <= (t->p0[1]-velocity[i][1])) velocity[i][1]*=-1;
        //     Vec2 d = velocity[i]*dt;
        //     t->movePoint(i,d);
        // }
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