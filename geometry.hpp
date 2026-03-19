// geometry.hpp
#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const double pi = 3.14159265358979323846;

bool equal(double num_1, double num_2) {
    return std::abs(num_1 - num_2) < 1e-9;
}

int sign(double num) {
    if (num > 0) return 1;
    if (num < 0) return -1;
    return 0;
}

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return equal(x, other.x) && equal(y, other.y);
    }
    bool operator!=(const Point& other) const { return !(*this == other); }
};

double length(const Point& p1, const Point& p2) {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

double angle(const Point& p1, const Point& p2, const Point& p3) {
    double a = length(p1, p2);
    double b = length(p2, p3);
    double c = length(p1, p3);
    return std::acos((a*a + b*b - c*c) / (2*a*b));
}

Point mid(const Point& p1, const Point& p2) {
    return Point((p1.x + p2.x)/2, (p1.y + p2.y)/2);
}

Point point_rotate(const Point& p, const Point& center, double angle) {
    double dx = p.x - center.x;
    double dy = p.y - center.y;
    return Point(center.x + dx*std::cos(angle) - dy*std::sin(angle),
                 center.y + dx*std::sin(angle) + dy*std::cos(angle));
}

double dot_product(double vx1, double vy1, double vx2, double vy2) {
    return vx1*vx2 + vy1*vy2;
}

class Line {
public:
    double A, B, C; // Ax + By + C = 0

    Line(const Point& p1, const Point& p2)
        : A(p2.y - p1.y), B(p1.x - p2.x), C(-p1.x*p2.y + p1.y*p2.x) {}

    Line(double k, double b) : A(k), B(-1), C(b) {} // y = kx + b

    Line(const Point& p, double k) : A(-1), B(k), C(p.y - k*p.x) {} // через точку и угловой коэффициент

    Line(double a, double b, double c) : A(a), B(b), C(c) {}

    bool operator==(const Line& other) const {
        if (equal(A, 0) && equal(B, 0)) return false; // не линия
        double factor = 0;
        if (!equal(A, 0) && !equal(other.A, 0)) factor = A / other.A;
        else if (!equal(B, 0) && !equal(other.B, 0)) factor = B / other.B;
        else if (!equal(C, 0) && !equal(other.C, 0)) factor = C / other.C;
        else return false;

        return equal(B, other.B * factor) && equal(C, other.C * factor);
    }

    bool operator!=(const Line& other) const { return !(*this == other); }
};

Point point_reflect(const Point& p, const Line& l) {
    double d = l.A * p.x + l.B * p.y + l.C;
    double t = -2 * d / (l.A*l.A + l.B*l.B);
    return Point(p.x + t * l.A, p.y + t * l.B);
}

Point intersection(const Line& l1, const Line& l2) {
    double det = l1.A * l2.B - l2.A * l1.B;
    if (equal(det, 0)) throw std::runtime_error("Lines are parallel");
    double x = (l1.B * l2.C - l2.B * l1.C) / det;
    double y = (l2.A * l1.C - l1.A * l2.C) / det;
    return Point(x, y);
}

class Shape {
public:
    virtual ~Shape() = default;
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& other) const = 0;
    bool operator!=(const Shape& other) const { return !(*this == other); }
    virtual bool isCongruentTo(const Shape& other) = 0;
    virtual bool isSimilarTo(const Shape& other) = 0;
    virtual bool containsPoint(const Point& p) = 0;

    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflect(const Point& center) = 0;
    virtual void reflect(const Line& axis) = 0;
    virtual void scale(const Point& center, double coeff) = 0;
};

class Polygon : public Shape {
protected:
    std::vector<Point> vertices_;

public:
    Polygon(const std::vector<Point>& verts) : vertices_(verts) {}

    template<typename... Points>
    Polygon(Points... points) : vertices_{points...} {}

    Polygon(const Polygon&) = default;
    Polygon& operator=(const Polygon&) = default;

    size_t verticesCount() const { return vertices_.size(); }
    std::vector<Point> getVertices() const { return vertices_; }

    double perimeter() const override {
        double per = 0;
        size_t n = verticesCount();
        for (size_t i = 0; i < n; ++i)
            per += length(vertices_[i], vertices_[(i+1)%n]);
        return per;
    }

    double area() const override {
        double area = 0;
        size_t n = verticesCount();
        for (size_t i = 0; i < n; ++i)
            area += vertices_[i].x * vertices_[(i+1)%n].y - vertices_[(i+1)%n].x * vertices_[i].y;
        return std::abs(area) / 2;
    }

    bool operator==(const Shape& other) const override {
        auto poly = dynamic_cast<const Polygon*>(&other);
        if (!poly || poly->verticesCount() != verticesCount()) return false;
        const auto& v2 = poly->getVertices();
        size_t n = verticesCount();

        auto check = [&](size_t start, bool reverse) {
            for (size_t i = 0; i < n; ++i) {
                size_t j = reverse ? (start - i + n) % n : (start + i) % n;
                if (vertices_[i] != v2[j]) return false;
            }
            return true;
        };

        for (size_t start = 0; start < n; ++start) {
            if (vertices_[0] == v2[start]) {
                if (check(start, false) || check(start, true))
                    return true;
            }
        }
        return false;
    }

    bool isCongruentTo(const Shape& other) override {
        // упрощённо: сравниваем длины сторон и углы (для выпуклых многоугольников)
        auto poly = dynamic_cast<const Polygon*>(&other);
        if (!poly || poly->verticesCount() != verticesCount()) return false;
        auto sides1 = getSides();
        auto sides2 = poly->getSides();
        auto angles1 = getAngles();
        auto angles2 = poly->getAngles();
        size_t n = sides1.size();

        auto check = [&](size_t start, bool reverse) {
            for (size_t i = 0; i < n; ++i) {
                size_t j = reverse ? (start - i + n) % n : (start + i) % n;
                if (!equal(sides1[i], sides2[j]) || !equal(angles1[i], angles2[j]))
                    return false;
            }
            return true;
        };

        for (size_t start = 0; start < n; ++start) {
            if (check(start, false) || check(start, true))
                return true;
        }
        return false;
    }

    bool isSimilarTo(const Shape& other) override {
        auto poly = dynamic_cast<const Polygon*>(&other);
        if (!poly || poly->verticesCount() != verticesCount()) return false;
        auto sides1 = getSides();
        auto sides2 = poly->getSides();
        auto angles1 = getAngles();
        auto angles2 = poly->getAngles();
        size_t n = sides1.size();

        auto check = [&](size_t start, bool reverse) {
            double ratio = sides1[0] / sides2[start];
            for (size_t i = 0; i < n; ++i) {
                size_t j = reverse ? (start - i + n) % n : (start + i) % n;
                if (!equal(angles1[i], angles2[j]) || !equal(sides1[i], sides2[j] * ratio))
                    return false;
            }
            return true;
        };

        for (size_t start = 0; start < n; ++start) {
            if (equal(angles1[0], angles2[start])) {
                if (check(start, false) || check(start, true))
                    return true;
            }
        }
        return false;
    }

    bool containsPoint(const Point& p) override {
        // ray casting algorithm
        bool inside = false;
        size_t n = verticesCount();
        for (size_t i = 0, j = n-1; i < n; j = i++) {
            if (((vertices_[i].y > p.y) != (vertices_[j].y > p.y)) &&
                (p.x < (vertices_[j].x - vertices_[i].x) * (p.y - vertices_[i].y) / (vertices_[j].y - vertices_[i].y) + vertices_[i].x))
                inside = !inside;
        }
        return inside;
    }

    void rotate(const Point& center, double angle) override {
        for (auto& v : vertices_)
            v = point_rotate(v, center, angle);
    }

    void reflect(const Point& center) override {
        for (auto& v : vertices_)
            v = Point(2*center.x - v.x, 2*center.y - v.y);
    }

    void reflect(const Line& axis) override {
        for (auto& v : vertices_)
            v = point_reflect(v, axis);
    }

    void scale(const Point& center, double coeff) override {
        for (auto& v : vertices_)
            v = Point(center.x + coeff*(v.x - center.x),
                      center.y + coeff*(v.y - center.y));
    }

    bool isConvex() const {
        size_t n = verticesCount();
        if (n <= 3) return true;
        int turn = 0;
        for (size_t i = 0; i < n; ++i) {
            const Point& a = vertices_[i];
            const Point& b = vertices_[(i+1)%n];
            const Point& c = vertices_[(i+2)%n];
            double dx1 = b.x - a.x, dy1 = b.y - a.y;
            double dx2 = c.x - b.x, dy2 = c.y - b.y;
            int s = sign(dx1*dy2 - dy1*dx2);
            if (turn == 0) turn = s;
            else if (s * turn < 0) return false;
        }
        return true;
    }

private:
    std::vector<double> getSides() const {
        std::vector<double> res;
        size_t n = verticesCount();
        for (size_t i = 0; i < n; ++i)
            res.push_back(length(vertices_[i], vertices_[(i+1)%n]));
        return res;
    }

    std::vector<double> getAngles() const {
        std::vector<double> res;
        size_t n = verticesCount();
        for (size_t i = 0; i < n; ++i) {
            const Point& a = vertices_[(i-1+n)%n];
            const Point& b = vertices_[i];
            const Point& c = vertices_[(i+1)%n];
            res.push_back(angle(a, b, c));
        }
        return res;
    }
};

class Ellipse : public Shape {
protected:
    Point f1_, f2_;
    double dist_sum_; // сумма расстояний до фокусов = 2a
    double a_; // большая полуось
    double b_; // малая полуось
    double c_; // расстояние от центра до фокуса

    void compute_axes() {
        a_ = dist_sum_ / 2;
        c_ = length(f1_, f2_) / 2;
        b_ = std::sqrt(a_*a_ - c_*c_);
    }

public:
    Ellipse(const Point& f1, const Point& f2, double sum_dist)
        : f1_(f1), f2_(f2), dist_sum_(sum_dist) {
        compute_axes();
    }

    std::pair<Point, Point> focuses() const { return {f1_, f2_}; }
    double eccentricity() const { return c_ / a_; }
    Point center() const { return mid(f1_, f2_); }

    double perimeter() const override {
        // приближённая формула Рамануджана
        return pi * (3*(a_+b_) - std::sqrt((3*a_+b_)*(a_+3*b_)));
    }

    double area() const override { return pi * a_ * b_; }

    bool operator==(const Shape& other) const override {
        auto e = dynamic_cast<const Ellipse*>(&other);
        if (!e) return false;
        return (f1_ == e->f1_ && f2_ == e->f2_ && equal(dist_sum_, e->dist_sum_)) ||
               (f1_ == e->f2_ && f2_ == e->f1_ && equal(dist_sum_, e->dist_sum_));
    }

    bool isCongruentTo(const Shape& other) override {
        auto e = dynamic_cast<const Ellipse*>(&other);
        if (!e) return false;
        return equal(length(f1_, f2_), length(e->f1_, e->f2_)) &&
               equal(dist_sum_, e->dist_sum_);
    }

    bool isSimilarTo(const Shape& other) override {
        auto e = dynamic_cast<const Ellipse*>(&other);
        if (!e) return false;
        double ratio1 = a_ / b_;
        double ratio2 = e->a_ / e->b_;
        return equal(ratio1, ratio2) || equal(ratio1, 1.0/ratio2);
    }

    bool containsPoint(const Point& p) override {
        double d1 = length(p, f1_);
        double d2 = length(p, f2_);
        return d1 + d2 <= dist_sum_ + 1e-9;
    }

    void rotate(const Point& center, double angle) override {
        f1_ = point_rotate(f1_, center, angle);
        f2_ = point_rotate(f2_, center, angle);
        compute_axes();
    }

    void reflect(const Point& center) override {
        f1_ = Point(2*center.x - f1_.x, 2*center.y - f1_.y);
        f2_ = Point(2*center.x - f2_.x, 2*center.y - f2_.y);
        compute_axes();
    }

    void reflect(const Line& axis) override {
        f1_ = point_reflect(f1_, axis);
        f2_ = point_reflect(f2_, axis);
        compute_axes();
    }

    void scale(const Point& center, double coeff) override {
        f1_ = Point(center.x + coeff*(f1_.x - center.x),
                    center.y + coeff*(f1_.y - center.y));
        f2_ = Point(center.x + coeff*(f2_.x - center.x),
                    center.y + coeff*(f2_.y - center.y));
        dist_sum_ *= coeff;
        compute_axes();
    }
};

class Circle : public Ellipse {
public:
    Circle(const Point& center, double radius)
        : Ellipse(center, center, 2*radius) {}

    double radius() const { return a_; }

    double perimeter() const override { return 2 * pi * radius(); }
    double area() const override { return pi * radius() * radius(); }

    bool operator==(const Shape& other) const override {
        auto c = dynamic_cast<const Circle*>(&other);
        if (!c) return false;
        return center() == c->center() && equal(radius(), c->radius());
    }

    void scale(const Point& center, double coeff) override {
        Ellipse::scale(center, coeff);
        // остаётся окружностью, так как фокусы совпадают
    }
};

class Rectangle : public Polygon {
public:
    Rectangle(const Point& A, const Point& C, double ratio) : Polygon(computeVertices(A, C, ratio)) {}

    Point center() const {
        auto v = getVertices();
        return mid(v[0], v[2]);
    }

    std::pair<Line, Line> diagonals() const {
        auto v = getVertices();
        return {Line(v[0], v[2]), Line(v[1], v[3])};
    }

private:
    static std::vector<Point> computeVertices(const Point& A, const Point& C, double ratio) {
        double w = ratio; // отношение смежных сторон (большая/меньшая)
        double diag = length(A, C);
        double h = std::sqrt(diag*diag / (1 + w*w));
        double w_side = w * h;

        // вектор диагонали
        double dx = C.x - A.x;
        double dy = C.y - A.y;
        double len = std::sqrt(dx*dx + dy*dy);
        dx /= len; dy /= len;

        // перпендикуляр
        double nx = -dy;
        double ny = dx;

        // ориентация: короткая сторона слева от диагонали
        double dp = dot_product(dx, dy, ny, -nx);
        if (dp < 0) { nx = -nx; ny = -ny; }

        Point B(A.x + nx * h, A.y + ny * h);
        Point D(2*mid(A,C).x - B.x, 2*mid(A,C).y - B.y);
        return {A, B, C, D};
    }
};

class Square : public Rectangle {
public:
    Square(const Point& A, const Point& C) : Rectangle(A, C, 1.0) {}

    Circle circumscribedCircle() const {
        auto v = getVertices();
        double r = length(v[0], v[2]) / 2;
        return Circle(center(), r);
    }

    Circle inscribedCircle() const {
        auto v = getVertices();
        double r = length(v[0], v[1]) / 2;
        return Circle(center(), r);
    }
};

class Triangle : public Polygon {
public:
    Triangle(const Point& A, const Point& B, const Point& C) : Polygon(A, B, C) {}

    Circle circumscribedCircle() const {
        auto v = getVertices();
        double a = length(v[1], v[2]);
        double b = length(v[0], v[2]);
        double c = length(v[0], v[1]);
        double R = (a * b * c) / (4 * area());

        // центр описанной окружности — пересечение серединных перпендикуляров
        Line s1 = Line(v[0], v[1]);
        Line s2 = Line(v[1], v[2]);
        Point m1 = mid(v[0], v[1]);
        Point m2 = mid(v[1], v[2]);
        Line p1(-s1.B, s1.A, s1.B*m1.x - s1.A*m1.y);
        Line p2(-s2.B, s2.A, s2.B*m2.x - s2.A*m2.y);
        Point O = intersection(p1, p2);
        return Circle(O, R);
    }

    Circle inscribedCircle() const {
        auto v = getVertices();
        double a = length(v[1], v[2]);
        double b = length(v[0], v[2]);
        double c = length(v[0], v[1]);
        double p = (a + b + c) / 2;
        double r = area() / p;

        double x = (a*v[0].x + b*v[1].x + c*v[2].x) / (a+b+c);
        double y = (a*v[0].y + b*v[1].y + c*v[2].y) / (a+b+c);
        return Circle(Point(x, y), r);
    }

    Point centroid() const {
        auto v = getVertices();
        return Point((v[0].x + v[1].x + v[2].x)/3,
                     (v[0].y + v[1].y + v[2].y)/3);
    }

    Point orthocenter() const {
        auto v = getVertices();
        Line ab(v[0], v[1]);
        Line bc(v[1], v[2]);
        Line ha(-ab.B, ab.A, ab.B*v[2].x - ab.A*v[2].y);
        Line hb(-bc.B, bc.A, bc.B*v[0].x - bc.A*v[0].y);
        return intersection(ha, hb);
    }

    Line EulerLine() const {
        return Line(centroid(), orthocenter());
    }

    Circle ninePointsCircle() const {
        auto v = getVertices();
        Point m1 = mid(v[0], v[1]);
        Point m2 = mid(v[1], v[2]);
        Point m3 = mid(v[2], v[0]);
        return Triangle(m1, m2, m3).circumscribedCircle();
    }
};

#endif // GEOMETRY_HPP
