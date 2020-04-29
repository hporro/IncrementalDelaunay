#include <string>
#include <iostream>
#include "../extern/tinytest.h"
#include "../src/delaunay.h"
#include "../src/point_generator.h"

#define POINTS_NUMBER 100

Vec2 p0 = Vec2(-0.9,-0.9);
Vec2 p1 = Vec2(0.9,-0.9);
Vec2 p2 = Vec2(0.0,0.9);

std::vector<Vec2> points;

//checks that every point is at the left or in every edge of the bounding triangle
void test_isLeft(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int i=0;i<points.size();i++){
        ASSERT("",mightBeLeft(p2-p1,points[i]-p1));
        ASSERT("",mightBeLeft(p1-p0,points[i]-p0));
        ASSERT("",mightBeLeft(p0-p2,points[i]-p2));
    }
}

//checks that every triangle in the triangulation os ccw oriented
void test_CCW(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int f=0;f<t.maxTriangles;f++){
        ASSERT("",t.isCCW(f));
    }
}

//checks that it finds only one triangle which contains a point
//or one or two triangles which has it in a edge.
void test_isInside(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    std::vector<Vec2> points2 = POINT_GENERATOR::gen_points_triangle(2,p0,p1,p2);
    for(int i=0;i<points2.size();i++){
        bool a = false;
        bool b = false;
        int c = 0;
        for(int f=0;f<t.maxTriangles;f++){
            if(t.isInside(f,points[i])){ a = true;}
            if(t.isInEdge(f,points[i])){ a = true; b = true; c++;}
        }
        if(c==0 && b)a=false;
        if(c>2 && b)a=false;
        ASSERT("",a);
    }
}

//checks that each triangle is in it's triangle's neighbours list
void test_check_integrity(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int i=0;i<t.tcount;i++){
        ASSERT("",t.integrity(i));
    }
}

void all_points_check(){
    //std::cout << t.incount << " " << t.edgecount << " " << t.oedgecount << std::endl;
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    ASSERT_EQUALS(POINTS_NUMBER,t.incount+t.oedgecount+t.edgecount);
}

int main(){
    points = POINT_GENERATOR::gen_points_triangle(POINTS_NUMBER,p0,p1,p2);
    RUN(test_isLeft);
    RUN(test_CCW);
    RUN(test_check_integrity);
    RUN(all_points_check);
    return TEST_REPORT();
}