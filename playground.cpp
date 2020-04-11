#include <iostream>

#include "src/point_generator.h"

int main(){
    Vec2 p0 = Vec2(-0.5,-0.5);
    Vec2 p1 = Vec2(0.5,-0.5);
    Vec2 p2 = Vec2(0.0,0.5);
    std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(2,p0,p1,p2);
    //POINT_GENERATOR::print_points(points);
    auto a = &points[1];

    //gen triangulation

    Triangulation tri({Vec2(-0.27614,-0.0522804)},points.size(),p0,p1,p2);
    tri.print();

    return 0;
}