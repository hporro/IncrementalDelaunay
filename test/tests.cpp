#include "../extern/tinytest.h"
#include "../src/delaunay.h"
#include "../src/point_generator.h"

Vec2 p0 = Vec2(-900,-900);
Vec2 p1 = Vec2(900,-900);
Vec2 p2 = Vec2(0,900);

std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(1000,p0,p1,p2);

Triangulation t(points,points.size(),p0,p1,p2);

//checks that every point is at the left or in every edge of the bounding triangle
void test_isLeft(){
    for(int i=0;i<points.size();i++){
        ASSERT("",mightBeLeft(p2-p1,points[i]-p1));
        ASSERT("",mightBeLeft(p1-p0,points[i]-p0));
        ASSERT("",mightBeLeft(p0-p2,points[i]-p2));
    }
}

//checks that every triangle in the triangulation os ccw oriented
void test_CCW(){
    for(int f=0;f<t.triangles.size();f++){
        ASSERT("",t.isCCW(f));
    }
}

//checks that it finds only one triangle which contains a point
//or one or two triangles which has it in a edge.
void test_isInside(){
    std::vector<Vec2> points2 = POINT_GENERATOR::gen_points_triangle(100,p0,p1,p2);
    for(int i=0;i<points2.size();i++){
        bool a = false;
        bool b = false;
        int c = 0;
        for(int f=0;f<t.triangles.size();f++){
            if(t.isInside(f,points[i])){ a = true;}
            if(t.isInEdge(f,points[i])){ a = true; b = true; c++;}
        }
        if(c==0 && b)a=false;
        if(c>2 && b)a=false;
        ASSERT("",a);
    }
}

int main(){
    RUN(test_isLeft);
    RUN(test_CCW);
    RUN(test_isInside);
    return TEST_REPORT();
}