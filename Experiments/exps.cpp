#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "../src/constants.h"

#include "../src/delaunay.h"
#include "../src/point_generator.h" 

#include <ctime>

double take_time_triangulation(std::vector<Vec2> points, void func(std::vector<Vec2> points)){
	std::clock_t start;
    double duration;
    start = std::clock();
	func(points); // executes func
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	return duration;
}

// prints results to the std output
int main() {
	auto make_log_triangulation = [](std::vector<Vec2> points){
		Triangulation t(points,points.size(),true);
	};
	auto make_linear_triangulation = [](std::vector<Vec2> points){
		Triangulation t(points,points.size(),false);
	};

	std::vector<int> numPs {1,100,500,1000,2000,3000,5000,8000,10000};
	std::vector<double> logs;
	std::vector<double> lins;

	for(auto numP: numPs){
		Vec2 p10 = Vec2{-0.8,-0.8};
		Vec2 p11 = Vec2{0.8,-0.8 };
		Vec2 p12 = Vec2{0.8,0.8  };
		Vec2 p13 = Vec2{-0.8,0.8 };
		std::vector<Vec2> points = POINT_GENERATOR::gen_points_square(numP,p10,p11,p12,p13);

		logs.push_back(take_time_triangulation(points,make_log_triangulation));
		lins.push_back(take_time_triangulation(points,make_linear_triangulation));
	}
	std::cout << "numP = [" << numPs[0];
	for(int i=1;i<(int)numPs.size();i++){
		std::cout << "," << numPs[i] << " ";
	}
	std::cout << "]\n";

	std::cout << "logs = [" << logs[0];
	for(int i=1;i<(int)logs.size();i++){
		std::cout << "," << logs[i] << " ";
	}
	std::cout << "]\n";

	std::cout << "lins = [" << lins[0];
	for(int i=1;i<(int)lins.size();i++){
		std::cout << "," << lins[i] << " ";
	}
	std::cout << "]\n";
}