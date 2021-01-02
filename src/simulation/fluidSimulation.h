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

class FluidSimulation {
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
    int* state_code; // particle state code {0=dont move, 1=water,}

    FluidSimulation(Triangulation *t, REAL maxVel) : t(t), numP(t->vcount), maxVel(maxVel) {
        velocity = new Vec2[t->maxVertices];
        t->velocity = this->velocity;
        this->numP = t->vcount;

        dt = 1.0/60.0;
        // g = 0.9;
        g = 0.0;

        h = 35;
        p0 = 6;
        k = 0.005;
        P = new REAL[numP];
        p = new REAL[numP];

        p_near = new REAL[numP];
        k_near = 0.4;
        P_near = new REAL[numP];

        dx = new Vec2[numP];

        state_code = new int[numP];

        for(int i=0;i<numP;i++){
            velocity[i] = Vec2{0,0};
            state_code[i] = 1;
        }
    }

    ~FluidSimulation(){
        delete velocity;
    }

    void step(REAL dt){
        for(int i=0;i<numP;i++){
            p[i] = 0;
            p_near[i] = 0;
        }

        for(int i=0;i<numP;i++) {
            std::set<int> neighbours = t->getFRNN(i,h);
            for (int j: neighbours) {
                REAL q = hmod(t->vertices[i].pos - t->vertices[j].pos) / h;
                if (q < 1 && i!=j) {
                    p[i] += pow2(1 - q);
                    p_near[i] += pow3(1 - q);
                }
            }
            P[i] = k * (p[i] - p0);
            P_near[i] = k_near * p_near[i];
            dx[i] = Vec2{0, 0};
            for (int j: neighbours) {
                REAL q = hmod(t->vertices[i].pos - t->vertices[j].pos) / h;
                if (q < 1 && i!=j) {
                    Vec2 rij = (t->vertices[j].pos - t->vertices[i].pos);
                    normalize(rij);
                    Vec2 D = rij * (REAL)(P[i] * (1 - q) + P_near[i] * pow2(1 - q));
                    D /= 2.0;
                    dx[i] -= D;
                }
            }
        }
        for(int i=4;i<numP;i++)if(state_code[i]){
            t->movePoint(i,dx[i] + velocity[i]*dt);
            velocity[i] = dx[i];
            velocity[i] -= Vec2{0,g};
            velocity[i] /= dt;
        }

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