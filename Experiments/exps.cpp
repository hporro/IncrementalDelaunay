#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "../src/constants.h"

#include "../src/delaunay.h"
#include "../src/point_generator.h" 

Triangulation gen_triangulation(int numP, float width, float height) {
	std::vector<Vec2> points = POINT_GENERATOR::gen_points_square(numP, Vec2(0.0, 0.0), Vec2(width, 0.0), Vec2(width, height), Vec2(height, 0.0));
	return Triangulation(points, numP);
}

int main() {
	gen_triangulation(100000, 10, 10);
}