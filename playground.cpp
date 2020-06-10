#include <iostream>
#include <math.h>

class foo {
public:
    int *a;
};

int main(){
    foo *a = new foo;
    a->a = new int;
    *a->a = 3;
    delete a->a;
    a->a = new int;
    *a->a = 3;
    return 0;
}