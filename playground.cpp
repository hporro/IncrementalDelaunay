#include <iostream>

#include "src/point_generator.h"

int main(){
    Vec2 p1 = Vec2(10.0,10.0);
    Vec2 p2 = Vec2(20.0,10.0);
    Vec2 p3 = Vec2(20.0,20.0);
    std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(2,p1,p2,p3);
    POINT_GENERATOR::print_points(points);
    auto a = &points[1];

    //gen triangulation
    Triangulation tri(points,points.size(),p1,p2,p3);
    
    return 0;
}