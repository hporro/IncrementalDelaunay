#include <vector>
#include <random>
#include <iostream>

#include "constants.h"
#include "delaunay.h"

namespace POINT_GENERATOR {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist01(0.0,1.0);

    template<class T>
    T max(T a,T b){
        if(a>b)return a;
        return b;
    }

    template<class T>
    T abs(T a){
        if(a>0)return a;
        return -a;
    }

    Vec2 gen_point(float a, float b, float c, Vec2 p0, Vec2 p1, Vec2 p2){
        assert(a+b+c-1<EPS);
        return Vec2(p0.x*a+p1.x*b+p2.x*c,p0.y*a+p1.y*b+p2.y*c);
    }

    //generates n points within a triangle
    //those might be in the 
    std::vector<Vec2> gen_points_triangle(int n, Vec2 p0, Vec2 p1, Vec2 p2){
        std::vector<Vec2> points(n);
        for(auto i=0;i<n;i++){
            float a = dist01(rng);
            float b = max<float>(0.0,abs(dist01(rng)-a));
            float c = 1.0-a-b;
            points[i] = gen_point(a,b,c,p0,p1,p2);
        }
        return points;
    }

    void print_points(std::vector<Vec2> points){
        for(auto p : points){
            std::cout << p.x << " " << p.y << std::endl;
        }
    }
}