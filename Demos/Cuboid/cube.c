#include <stdio.h>
#include <stdbool.h>


typedef struct {
    double a;
    double b;
    double c;
} Cuboid;

bool set_size(Cuboid *shape, double a, double b, double c) {
    if (a > 0 && b > 0 && c > 0) {
        shape->a = a;
        shape->b = b;
        shape->c = c;
        return true;
    }
    return false;
}

double calc_volume(Cuboid shape) {
    return shape.a * shape.b * shape.c;
}

double calc_surface(Cuboid shape) {
    return 2 * (shape.a * shape.b + shape.a * shape.c + shape.b * shape.c);
}

bool has_square_face(Cuboid shape) {
    return (shape.a == shape.b || shape.a == shape.c || shape.b == shape.c);
}

int main() {
    Cuboid myBrick;
    
    if (set_size(&myBrick, 10.0, 5.0, 10.0)) {
        printf("Teglatest adatai sikeresen beallitva\n");
        
        printf("Felszin: %.2f\n", calc_surface(myBrick));
        printf("Terfogat: %.2f\n", calc_volume(myBrick));
        
        if (has_square_face(myBrick)) {
            printf("A teglatestnek van legalabb egy negyzet alaku lapja\n");
        } else {
            printf("A teglatestnek nincs negyzet alaku lapja\n");
        }
    } else {
        printf("Hiba: Az elek hossza csak pozitiv szam lehet\n");
    }

    return 0;
}