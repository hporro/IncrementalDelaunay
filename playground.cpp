#include <iostream>

#include "src/point_generator.h"

int main(){
    Vec2 p1 = Vec2(10.0,10.0);
    Vec2 p2 = Vec2(20.0,10.0);
    Vec2 p3 = Vec2(20.0,20.0);
    
    std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(3,p1,p2,p3);
    POINT_GENERATOR::print_points(points);
    
    return 0;
}