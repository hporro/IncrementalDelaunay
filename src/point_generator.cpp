#include <vector>
#include <random>
#include <iostream>

#include "constants.h"
#include "delaunay.h"

namespace POINT_GENERATOR {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist01(0.0,1.0);

    Vec2 operator*(Vec2 v, float a){
        return Vec2(v.x*a,v.y*a);
    }

    Vec2 operator*(float a, Vec2 v){
        return Vec2(v.x*a,v.y*a);
    }

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
        assert(a+b+c-1.0<0.000001);
        return Vec2(p0.x*a+p1.x*b+p2.x*c,p0.y*a+p1.y*b+p2.y*c);
    }

    Vec2 gen_point(float a, float b, float c, float d, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3){
        assert(a+b+c+d-1.0<0.000001);
        return Vec2(p0.x*a+p1.x*b+p2.x*c+p3.x*d,p0.y*a+p1.y*b+p2.y*c+p3.y*d);
    }

    //generates n points within a triangle
    //those might be in the 
    std::vector<Vec2> gen_points_triangle(int n, Vec2 p0, Vec2 p1, Vec2 p2){
        std::vector<Vec2> points(n);
        for(auto i=0;i<n;i++){
            float r1 = dist01(rng);
            float r2 = dist01(rng);
            // https://stackoverflow.com/questions/4778147/sample-random-point-in-triangle
            points[i] = (1 - sqrt(r1)) * p0 + (sqrt(r1) * (1 - r2)) * p1 + (sqrt(r1) * r2) * p2;
        }
        return points;
    }

    std::vector<Vec2> gen_points_square(int n, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3){
        std::vector<Vec2> points(n);
        for(auto i=0;i<n;i++){
            float a = dist01(rng);
            float b = dist01(rng);
            points[i] = p0 + (p3-p0)*a + (p1-p0)*b;
        }
        return points;
    }

    void print_points(std::vector<Vec2> points){
        for(auto p : points){
            std::cout << p.x << " " << p.y << std::endl;
        }
    }

    std::vector<Vec2> gen_points_grid(int x, int y, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3){
        std::vector<Vec2> points(x*y);
        for(int i=0;i<x;i++){
            for(int j=0;j<y;j++){
                points[i*y+j] = p0 + (p1-p0)*(((float)i)/x) + (p2-p1)*((float)j/y);
            }
        }
        return points;
    }
}