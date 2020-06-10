//HELPER FUNCTIONS
double crossa(Vec2 a, Vec2 b){
    return a.x*b.y-a.y*b.x;
}

bool isLeft(Vec2 a, Vec2 b){
    return crossa(a,b) > IN_TRIANGLE_EPS;
}

bool isRight(Vec2 a, Vec2 b){
    return crossa(a,b) < IN_TRIANGLE_EPS;
}

bool mightBeLeft(Vec2 a, Vec2 b){
    return crossa(a,b) >= -IN_TRIANGLE_EPS;
}

template<class T>
T amin(T a, T b){
    if(a<b)return a;
    return b;
}

double det(double a, double b, double c, double d, double e, double f, double g, double h, double i){
    return a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
}

double inCircle(Vec2 a, Vec2 b, Vec2 c, Vec2 d){
    return det( a.x-d.x,a.y-d.y,(a.x-d.x)*(a.x-d.x)+(a.y-d.y)*(a.y-d.y),
                b.x-d.x,b.y-d.y,(b.x-d.x)*(b.x-d.x)+(b.y-d.y)*(b.y-d.y),
                c.x-d.x,c.y-d.y,(c.x-d.x)*(c.x-d.x)+(c.y-d.y)*(c.y-d.y));
}

//gives the square of the length of a Vec2
double sqrtLength(Vec2 v){
    return v.x*v.x + v.y*v.y;
}