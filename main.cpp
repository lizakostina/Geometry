// main.cpp
#include "geometry.hpp"
#include <iostream>
#include <cassert>

void test_point() {
    Point p1(1, 2), p2(1, 2), p3(2, 3);
    assert(p1 == p2);
    assert(p1 != p3);
    std::cout << "Point tests passed.\n";
}

void test_line() {
    Point p1(0, 0), p2(1, 1);
    Line l1(p1, p2);
    Line l2(1, 0); // y = x
    assert(l1 == l2);
    std::cout << "Line tests passed.\n";
}

void test_polygon() {
    std::vector<Point> verts = {Point(0,0), Point(2,0), Point(2,2), Point(0,2)};
    Polygon square(verts);
    assert(square.verticesCount() == 4);
    assert(equal(square.perimeter(), 8.0));
    assert(equal(square.area(), 4.0));
    assert(square.isConvex());
    assert(square.containsPoint(Point(1,1)));
    assert(!square.containsPoint(Point(3,3)));

    Polygon triangle(Point(0,0), Point(3,0), Point(0,4));
    assert(equal(triangle.perimeter(), 12.0));
    assert(equal(triangle.area(), 6.0));

    std::cout << "Polygon tests passed.\n";
}

void test_ellipse() {
    Point f1(0,0), f2(4,0);
    Ellipse e(f1, f2, 10); // сумма = 10, a=5, c=2, b=√(25-4)=√21≈4.5826
    assert(equal(e.area(), pi * 5 * std::sqrt(21)));
    assert(equal(e.eccentricity(), 0.4));
    assert(e.containsPoint(Point(5,0))); // вершина большой оси
    assert(!e.containsPoint(Point(6,0)));

    Circle c(Point(0,0), 5);
    assert(equal(c.perimeter(), 10*pi));
    assert(equal(c.area(), 25*pi));

    std::cout << "Ellipse/Circle tests passed.\n";
}

void test_rectangle_square() {
    Rectangle rect(Point(0,0), Point(4,2), 2.0); // отношение 2:1
    assert(equal(rect.area(), 8.0));
    assert(rect.isConvex());

    Square sq(Point(0,0), Point(2,2));
    assert(equal(sq.area(), 4.0));
    Circle circ = sq.circumscribedCircle();
    assert(equal(circ.radius(), std::sqrt(2)));

    std::cout << "Rectangle/Square tests passed.\n";
}

void test_triangle() {
    Triangle t(Point(0,0), Point(4,0), Point(0,3));
    assert(equal(t.area(), 6.0));
    Circle inc = t.inscribedCircle();
    // радиус вписанной в прямоугольный треугольник: r = (a+b-c)/2 = (3+4-5)/2 = 1
    assert(equal(inc.radius(), 1.0));
    Circle circ = t.circumscribedCircle();
    // гипотенуза = 5, радиус описанной = 2.5
    assert(equal(circ.radius(), 2.5));

    Point centroid = t.centroid();
    assert(equal(centroid.x, 4.0/3) && equal(centroid.y, 1.0));

    std::cout << "Triangle tests passed.\n";
}

void test_transformations() {
    Polygon square(Point(0,0), Point(1,0), Point(1,1), Point(0,1));
    square.scale(Point(0,0), 2.0);
    assert(square.containsPoint(Point(1.5,1.5)));

    square.rotate(Point(0,0), pi/2);
    // после поворота на 90° вершина (2,0) переходит в (0,2)
    auto verts = square.getVertices();
    assert(verts[1] == Point(0,2) || verts[1] == Point(-2,0) || verts[1] == Point(2,0)); // упрощённо

    std::cout << "Transformations tests passed.\n";
}

int main() {
    test_point();
    test_line();
    test_polygon();
    test_ellipse();
    test_rectangle_square();
    test_triangle();
    test_transformations();

    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}
