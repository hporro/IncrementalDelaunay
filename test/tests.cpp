#include <string>
#include <iostream>
#include <queue>
#include "../extern/tinytest.h"
#include "../src/delaunay.h"
#include "../src/point_generator.h"

#define POINTS_NUMBER 1000 // set the number of points will have the test triangulation

Vec2 p0 = Vec2(-89,-89);
Vec2 p1 = Vec2(89,-89);
Vec2 p2 = Vec2(0,89);

std::vector<Vec2> points;

//checks that every point is at the left or in every edge of the bounding triangle
void test_isLeft(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int i=3;i<points.size();i++){
        ASSERT_TRUE(isLeft(p2-p1,points[i]-p1));
        ASSERT_TRUE(isLeft(p1-p0,points[i]-p0));
        ASSERT_TRUE(isLeft(p0-p2,points[i]-p2));
    }
}

//checks that every triangle in the triangulation os ccw oriented
void test_CCW(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int f=0;f<t.tcount;f++){
        ASSERT_TRUE(t.isCCW(f));
    }
}

//checks that it finds only one triangle which contains a point
//or one or two triangles which has it in a edge.
void test_isInside(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    std::vector<Vec2> points2 = POINT_GENERATOR::gen_points_triangle(2000,p0,p1,p2);
    for(int i=0;i<points2.size();i++){
        bool a = false;
        bool b = false;
        int c = 0;
        for(int f=0;f<t.tcount;f++){
            if(t.isInside(f,points[i])){ a = true;}
            if(t.isInEdge(f,points[i])){ a = true; b = true; c++;}
        }
        if(c==0 && b)a=false;
        if(c>2 && b)a=false;
        ASSERT_TRUE(a);
    }
}

//checks that all triangles are connected between them
void test_all_connected(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    std::vector<int> q;
    q.push_back(0);
    int *visited = new int[t.tcount];
    for(int i=0;i<t.tcount;i++)visited[i]=0;
    while(!q.empty()){
        int f = q[q.size()-1];
        q.pop_back();
        if(f!=-1 && !visited[f]){
            visited[f]=1;
            q.push_back(t.triangles[f].t[0]);
            q.push_back(t.triangles[f].t[1]);
            q.push_back(t.triangles[f].t[2]);
        }
    }
    for(int i=0;i<t.tcount;i++){
        ASSERT_EQUALS(visited[i],1);
    }
}

//checks that each triangle is in it's triangle's neighbours list
void test_check_integrity(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int i=0;i<t.tcount;i++){
        ASSERT_TRUE(t.integrity(i));
    }
}

//checks that each triangle shares 2 vertices with its neighbours
void test_sanity(){
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    for(int i=0;i<t.tcount;i++){
        ASSERT_TRUE(t.sanity(i));
    }
}

void all_points_check(){
    //std::cout << t.incount << " " << t.edgecount << " " << t.oedgecount << std::endl;
    Triangulation t = Triangulation(points,points.size(),p0,p1,p2);
    ASSERT_EQUALS(POINTS_NUMBER,t.incount+t.oedgecount+t.edgecount);
}

//checks that all edges obey the delaunay test
void delaunay_test() {
    Vec2 p10 = Vec2(-800, -800);
    Vec2 p11 = Vec2(800, -800);
    Vec2 p12 = Vec2(800, 800);
    Vec2 p13 = Vec2(-800, 800);
    points = POINT_GENERATOR::gen_points_square(POINTS_NUMBER,p10,p11,p12,p13);
    //points = POINT_GENERATOR::gen_points_grid((int)sqrt(POINTS_NUMBER), (int)sqrt(POINTS_NUMBER), p10, p11, p12, p13);
    Triangulation t = Triangulation(points, points.size(), p0, p1, p2);
    for (int i = 0; i < t.tcount; i++) { // all triangles
        for (int j = 0; j < 3; j++) { // each triangle neighbour
            int tj = t.triangles[i].t[j];
            if(tj!=-1)for (int k = 0; k < 3; k++) { // j triangle vertices
                int vj = t.triangles[tj].v[k];
                bool different = true;
                for (int l = 0; l < 3; l++) { // i triangle vertices
                    if (vj == t.triangles[i].v[l]) different = false;
                }
                if (different) {
                    Vec2 a = t.vertices[t.triangles[i].v[0]].pos;
                    Vec2 b = t.vertices[t.triangles[i].v[1]].pos;
                    Vec2 c = t.vertices[t.triangles[i].v[2]].pos;
                    Vec2 d = t.vertices[vj].pos;
                    float ff = inCircle(a, b, c, d);
                    ASSERT_TRUE(ff<0.01);
                }
            }
        }
    }
}

int main(){
    points = POINT_GENERATOR::gen_points_triangle(POINTS_NUMBER,p0,p1,p2);
    RUN(test_isLeft);
    RUN(test_CCW);
    RUN(test_check_integrity);
    RUN(test_all_connected);
    RUN(all_points_check);
    RUN(test_sanity);
    RUN(delaunay_test);
    return TEST_REPORT();
}