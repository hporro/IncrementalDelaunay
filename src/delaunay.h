#ifndef DELAUNAY_H
#define DELAUNAY_H

typedef float numType;

class Point;
class Vector;
class Face;

class Point {
public:
    numType x,y;
};

class Vector {
public:
    numType a,b;
};

class Face {
public:
    Point *a,*b,*c;
    Face *f1,*f2,*f3;
};

#endif