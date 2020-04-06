#include "constants.h"
#include "delaunay.h"

namespace POINT_GENERATOR {
    template<class T>
    T max(T a,T b);
    Vec2 gen_point(float a, float b, float c, Vec2 p0, Vec2 p1, Vec2 p3);
    std::vector<Vec2> gen_points_triangle(int n, Vec2 p0, Vec2 p1, Vec2 p3);
    void print_points(std::vector<Vec2> points);
}