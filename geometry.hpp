#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const double pi = 3.14159265;

bool equal(double num_1, double num_2) {
    if (std::abs(num_1 - num_2) < 0.000001) {
        return true;
    }
    return false;
}

int sign(double num) {
    if (num > 0) {
        return 1;
    }
    if (num < 0) {
        return -1;
    }
    return 0;
}

struct Point {
    double x;
    double y;
    Point(double x, double y) : x(x), y(y){};
    bool operator==(const Point& another) const {
        return (equal(x, another.x) && equal(y, another.y));
    }
    bool operator!=(const Point& another) const { return !(*this == another); }
};

double length(const Point& point_1, const Point& point_2) {
    return sqrt(pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2));
}

double angle(const Point& point_1, const Point& point_2, const Point& point_3) {
    double side_12 = length(point_1, point_2);
    double side_23 = length(point_2, point_3);
    double side_13 = length(point_1, point_3);
    return acos((side_12 * side_12 + side_23 * side_23 - side_13 * side_13) /
                (2 * side_12 * side_23));
}

Point mid(const Point& point_1, const Point& point_2) {
    return Point((point_1.x + point_2.x) / 2, (point_1.y + point_2.y) / 2);
}

Point point_rotate(const Point& point, const Point& center, double angle) {
    double new_x = center.x + (point.x - center.x) * cos(angle) -
                   (point.y - center.y) * sin(angle);
    double new_y = center.y + (point.x - center.x) * sin(angle) +
                   (point.y - center.y) * cos(angle);
    return Point(new_x, new_y);
}

double dot_product(double vx1, double vy1, double vx2, double vy2) {
    return vx1 * vx2 + vy1 * vy2;
}

class Line {
public:
    double coeff_x;
    double coeff_y;
    double free_coeff;

    Line(const Point& point_1, const Point& point_2)
            : coeff_x(point_2.y - point_1.y),
              coeff_y(point_1.x - point_2.x),
              free_coeff(-point_1.x * point_2.y + point_1.y * point_2.x) {}

    Line(double ang_coeff, double shift)
            : coeff_x(ang_coeff), coeff_y(-1), free_coeff(shift){};

    Line(const Point& point, double ang_coeff)
            : coeff_x(-1),
              coeff_y(ang_coeff),
              free_coeff(point.y - ang_coeff * point.x) {}
    Line(double A, double B, double C) : coeff_x(A), coeff_y(B), free_coeff(C) {}

    bool operator==(const Line& another) const {
        if (coeff_x == 0 && free_coeff == 0) {
            return another.coeff_x == another.free_coeff == 0;
        } else if (coeff_y == 0 && free_coeff == 0) {
            return another.coeff_y == another.free_coeff == 0;
        } else if (coeff_x == 0) {
            return equal(coeff_y / another.coeff_y,
                         free_coeff / another.free_coeff) &&
                   another.coeff_x == 0;
        } else if (coeff_y == 0) {
            return equal(coeff_x / another.coeff_x,
                         free_coeff / another.free_coeff) &&
                   another.coeff_y == 0;
        } else if (free_coeff == 0) {
            return equal(coeff_x / another.coeff_x, coeff_y / another.coeff_y) &&
                   another.free_coeff == 0;
        }
        return equal(coeff_x / another.coeff_x, coeff_y / another.coeff_y) &&
               equal(coeff_x / another.coeff_x, free_coeff / another.free_coeff);
    }

    bool operator!=(const Line& another) const { return !(*this == another); }
};

Point point_reflect(const Point& point, const Line& line) {
    double new_x =
            point.x -
            2 * line.coeff_x *
            (line.coeff_x * point.x + line.coeff_y * point.y + line.free_coeff) /
            (line.coeff_x * line.coeff_x + line.coeff_y * line.coeff_y);
    double new_y =
            point.y -
            2 * line.coeff_y *
            (line.coeff_x * point.x + line.coeff_y * point.y + line.free_coeff) /
            (line.coeff_x * line.coeff_x + line.coeff_y * line.coeff_y);
    return Point(new_x, new_y);
}

Point intersection(const Line& line_1, const Line& line_2) {
    double new_y =
            (line_1.free_coeff * line_2.coeff_x -
             line_2.free_coeff * line_1.coeff_x) /
            (line_1.coeff_x * line_2.coeff_y - line_2.coeff_x * line_1.coeff_y);
    double new_x =
            ((-line_1.coeff_y * new_y - line_1.free_coeff) / line_1.coeff_x);
    return Point(new_x, new_y);
}

class Shape {
public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& another) const = 0;
    virtual bool operator!=(const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape& another) = 0;
    virtual bool isSimilarTo(const Shape& another) = 0;
    virtual bool containsPoint(const Point& point) = 0;

    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflect(const Point& center) = 0;
    virtual void reflect(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;

    virtual ~Shape() = default;
};

class Polygon : public Shape {
private:
    std::vector<Point> vertices;

public:
    Polygon(const std::vector<Point>& vert) : vertices(vert){};
    template <class... Points>
    Polygon(Points&&... points) : vertices{std::forward<Points>(points)...} {}

    Polygon(Polygon& P) : vertices(P.getVertices()){};

    Polygon& operator=(const Polygon& P) {
        if (this != &P) {
            vertices = P.vertices;
        }
        return *this;
    }

    double perimeter() const override {
        double per = 0;
        for (size_t i = 0; i < verticesCount() - 1; ++i) {
            per += length(vertices[i], vertices[i + 1]);
        }
        per += length(vertices[verticesCount() - 1], vertices[0]);
        return per;
    }

    double area() const override {
        double area = 0;
        for (size_t i = 0; i < verticesCount() - 1; ++i) {
            area += vertices[i].x * vertices[i + 1].y;
            area -= vertices[i].y * vertices[i + 1].x;
        }
        area += vertices[verticesCount() - 1].x * vertices[0].y;
        area -= vertices[verticesCount() - 1].y * vertices[0].x;
        return std::abs(area / 2);
    }

    bool operator==(const Shape& another) const override {
        auto polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || polygon->verticesCount() != verticesCount()) {
            return false;
        }
        std::vector<Point> v1 = getVertices();
        std::vector<Point> v2 = polygon->getVertices();

        std::vector<size_t> poss;
        for (size_t i = 0; i < v2.size(); ++i) {
            if (v1[0] == v2[i]) {
                poss.push_back(i);
            }
        }
        if (poss.empty()) {
            return false;
        }

        bool flag;
        for (size_t i : poss) {
            flag = true;
            for (size_t j = 0; j < v1.size(); ++j) {
                if (v1[j] != v2[(i + j) % v1.size()]) {
                    flag = false;
                    break;
                }
            }
            if (flag) return true;
        }

        for (size_t i : poss) {
            flag = true;
            for (size_t j = 0; j < v1.size(); ++j) {
                size_t k = (i - j + v1.size()) % v1.size();
                if (v1[j] != v2[k]) {
                    flag = false;
                    break;
                }
            }
            if (flag) return true;
        }

        return false;
    }

    bool operator!=(const Shape& another) const override {
        return !(*this == another);
    }

    bool isCongruentTo(const Shape& another) override {
        auto polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || polygon->verticesCount() != verticesCount()) {
            return false;
        }

        std::vector<double> sides_1 = getSides();
        std::vector<double> sides_2 = polygon->getSides();
        std::vector<double> angles_1 = getAngles();
        std::vector<double> angles_2 = polygon->getAngles();
        size_t count = sides_1.size();
        std::vector<size_t> poss;

        for (size_t i = 0; i < count; ++i) {
            if (equal(sides_1[0], sides_2[i])) {
                poss.push_back(i);
            }
        }

        if (poss.empty()) {
            return false;
        }

        for (size_t i : poss) {
            bool flag = true;

            for (size_t j = 0; j < count; ++j) {
                if (!equal(angles_1[j], angles_2[(i + j) % count]) ||
                    !equal(sides_1[j], sides_2[(i + j) % count])) {
                    flag = false;
                    break;
                }
            }

            if (flag) {
                return true;
            }
        }

        for (size_t i = 0; i < count; ++i) {
            bool flag = true;

            for (size_t j = 0; j < count; ++j) {
                if (!equal(angles_1[j], angles_2[(i - j + count) % count]) ||
                    !equal(sides_1[j], sides_2[(i - j + count - 1) % count])) {
                    flag = false;
                    break;
                }
            }

            if (flag) {
                return true;
            }
        }

        return false;
    }

    bool isSimilarTo(const Shape& another) override {
        auto polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || polygon->verticesCount() != verticesCount()) {
            return false;
        }

        std::vector<double> sides_1 = getSides();
        std::vector<double> sides_2 = polygon->getSides();
        std::vector<double> angles_1 = getAngles();
        std::vector<double> angles_2 = polygon->getAngles();
        size_t count = sides_1.size();
        std::vector<size_t> poss;

        for (size_t i = 0; i < count; ++i) {
            if (equal(angles_1[0], angles_2[i])) {
                poss.push_back(i);
            }
        }
        if (poss.empty()) {
            return false;
        }

        for (size_t i : poss) {
            bool flag = true;
            double ratio = sides_1[0] / sides_2[i];

            for (size_t j = 0; j < count; ++j) {
                if (!equal(angles_1[j], angles_2[(i + j) % count]) ||
                    !equal(ratio, sides_1[j] / sides_2[(i + j) % count])) {
                    flag = false;
                    break;
                }
            }

            if (flag) {
                return true;
            }
        }

        for (size_t i = 0; i < count; ++i) {
            bool flag = true;
            double ratio = sides_1[0] / sides_2[(i + count - 1) % count];

            for (size_t j = 0; j < count; ++j) {
                if (!equal(angles_1[j], angles_2[(i - j + count) % count]) ||
                    !equal(ratio, sides_1[j] / sides_2[(i - j + count - 1) % count])) {
                    flag = false;
                    break;
                }
            }

            if (flag) {
                return true;
            }
        }

        return false;
    }

    bool containsPoint(const Point& point) override {
        size_t j = verticesCount() - 1;
        bool flag = false;
        for (size_t i = 0; i < verticesCount(); ++i) {
            if ((vertices[i].y < point.y && vertices[j].y >= point.y) ||
                (vertices[j].y < point.y && vertices[i].y >= point.y)) {
                if (vertices[j].y != vertices[i].y) {
                    double inter = vertices[i].x + (point.y - vertices[i].y) /
                                                   (vertices[j].y - vertices[i].y) *
                                                   (vertices[j].x - vertices[i].x);
                    if (inter < point.x) {
                        flag = !flag;
                    }
                }
            }
            j = i;
        }
        return flag;
    }

    void rotate(const Point& center, double angle) override {
        std::vector<Point> vert;
        for (size_t i = 0; i < verticesCount(); ++i) {
            double new_x = (vertices[i].x - center.x) * cos(angle) -
                           (vertices[i].y - center.y) * sin(angle) + center.x;
            double new_y = (vertices[i].x - center.x) * sin(angle) +
                           (vertices[i].y - center.y) * cos(angle) + center.y;
            vert.push_back(Point(new_x, new_y));
        }
        *this = Polygon(vert);
    }

    void reflect(const Point& center) override {
        std::vector<Point> vert;
        for (size_t i = 0; i < verticesCount(); ++i) {
            double new_x = 2 * center.x - vertices[i].x;
            double new_y = 2 * center.y - vertices[i].y;
            vert.push_back(Point(new_x, new_y));
        }
        *this = Polygon(vert);
    }

    void reflect(const Line& axis) override {
        std::vector<Point> vert;
        for (size_t i = 0; i < verticesCount(); ++i) {
            vert.push_back(point_reflect(vertices[i], axis));
        }
        *this = Polygon(vert);
    }

    void scale(const Point& center, double coefficient) override {
        std::vector<Point> vert;
        for (size_t i = 0; i < verticesCount(); ++i) {
            double new_x = center.x + coefficient * (vertices[i].x - center.x);
            double new_y = center.y + coefficient * (vertices[i].y - center.y);
            vert.push_back(Point(new_x, new_y));
        }
        *this = Polygon(vert);
    }

    size_t verticesCount() const { return vertices.size(); }

    std::vector<Point> getVertices() const { return vertices; }

private:
    std::vector<double> getSides() const {
        std::vector<double> sides;
        for (size_t i = 0; i < verticesCount(); ++i) {
            sides.push_back(length(vertices[i], vertices[(i + 1) % verticesCount()]));
        }
        return sides;
    }

    std::vector<double> getAngles() const {
        std::vector<double> angles;
        for (size_t i = 0; i < verticesCount(); ++i) {
            angles.push_back(
                    angle(vertices[(i - 1 + verticesCount()) % verticesCount()],
                          vertices[i], vertices[(i + 1) % verticesCount()]));
        }
        return angles;
    }

public:
    bool isConvex() {
        if (verticesCount() <= 3) {
            return true;
        }
        int flag = true;
        double turn_sign = 0;
        std::vector<Point> new_vert = vertices;
        new_vert.push_back(vertices[0]);
        new_vert.push_back(vertices[1]);
        for (size_t i = 0; i < new_vert.size() - 2; ++i) {
            double AB_x = new_vert[i + 1].x - new_vert[i].x;
            double AB_y = new_vert[i + 1].y - new_vert[i].y;
            double BC_x = new_vert[i + 2].x - new_vert[i + 1].x;
            double BC_y = new_vert[i + 2].y - new_vert[i + 1].y;
            if (turn_sign == 0) {
                turn_sign = sign(AB_x * BC_y - AB_y * BC_x);
            } else {
                if (sign(AB_x * BC_y - AB_y * BC_x) * turn_sign < 0) {
                    flag = false;
                    break;
                }
            }
        }

        return flag;
    }
};

class Ellipse : public Shape {
private:
    Point focus_1;
    Point focus_2;
    double sum_dist;
    double big_semi_axis = sum_dist / 2;
    double focus_dist =
            sqrt(pow(focus_2.x - focus_1.x, 2) + pow(focus_2.y - focus_1.y, 2)) / 2;
    double small_semi_axis = sqrt(pow(big_semi_axis, 2) - pow(focus_dist, 2));

public:
    Ellipse(const Point& focus_1, const Point& focus_2, double sum_dist)
            : focus_1(focus_1), focus_2(focus_2), sum_dist(sum_dist){};

    double perimeter() const override {
        return pi * (3 * (big_semi_axis + small_semi_axis) -
                     sqrt((3 * big_semi_axis + small_semi_axis) *
                          (big_semi_axis + 3 * small_semi_axis)));
    }

    double area() const override { return pi * big_semi_axis * small_semi_axis; }

    bool operator==(const Shape& another) const override {
        auto ellipse = dynamic_cast<const Ellipse*>(&another);
        if ((ellipse == nullptr) ||
            !((((focus_1 == ellipse->focus_1) && (focus_2 == ellipse->focus_2)) ||
               ((focus_1 == ellipse->focus_2) && (focus_2 == ellipse->focus_1))) &&
              (sum_dist == ellipse->sum_dist))) {
            return false;
        }
        return true;
    }

    bool operator!=(const Shape& another) const override {
        return !(*this == another);
    }

    bool isCongruentTo(const Shape& another) override {
        auto ellipse = dynamic_cast<const Ellipse*>(&another);
        return !((ellipse == nullptr) ||
                 (!equal(length(focus_1, focus_2),
                         length(ellipse->focus_1, ellipse->focus_2)) &&
                  (sum_dist == ellipse->sum_dist)));
    }

    bool isSimilarTo(const Shape& another) override {
        auto ellipse = dynamic_cast<const Ellipse*>(&another);
        return !(ellipse == nullptr ||
                 !(big_semi_axis / small_semi_axis ==
                   ellipse->big_semi_axis / ellipse->small_semi_axis ||
                   big_semi_axis / small_semi_axis ==
                   ellipse->small_semi_axis / ellipse->big_semi_axis));
    }

    bool containsPoint(const Point& point) override {
        Point cent = mid(focus_1, focus_2);
        double new_x = point.x - cent.x;
        double new_y = point.y - cent.y;
        if ((new_x * new_x) / (big_semi_axis * big_semi_axis) +
            (new_y * new_y) / (small_semi_axis * small_semi_axis) <=
            1) {
            return true;
        }
        return false;
    }

    void rotate(const Point& center, double angle) override {
        focus_1 = point_rotate(focus_1, center, angle);
        focus_2 = point_rotate(focus_2, center, angle);
    }

    void reflect(const Point& center) override {
        focus_1 = Point(2 * center.x - focus_1.x, 2 * center.y - focus_1.y);
        focus_2 = Point(2 * center.x - focus_2.x, 2 * center.y - focus_2.y);
    }

    void reflect(const Line& axis) override {
        focus_1 = point_reflect(focus_1, axis);
        focus_2 = point_reflect(focus_2, axis);
    }

    void scale(const Point& center, double coefficient) override {
        double new_x1 = center.x + coefficient * (focus_1.x - center.x);
        double new_y1 = center.y + coefficient * (focus_1.y - center.y);

        double new_x2 = center.x + coefficient * (focus_2.x - center.x);
        double new_y2 = center.y + coefficient * (focus_2.y - center.y);

        *this = Ellipse(Point(new_x1, new_y1), Point(new_x2, new_y2),
                        sum_dist * coefficient);
    }

    std::pair<Point, Point> focuses() { return std::pair{focus_1, focus_2}; }

    std::pair<Line, Line> directrices() {
        Line axis = Line(focus_1, focus_2);
        Point cent = mid(focus_1, focus_2);
        if (axis.coeff_y != 0) {
            double k = -axis.coeff_x / axis.coeff_y;
            double q = -axis.free_coeff / axis.coeff_y;
            double dis =
                    pow((-2 * cent.x - 2 * k * cent.y + 2 * k * q), 2) -
                    4 * (1 + pow(k, 2)) *
                    (pow(cent.x, 2) + pow(cent.y, 2) + pow(q, 2) - 2 * q * cent.y -
                     pow(big_semi_axis, 2) / eccentricity());
            double x1 = (-(-2 * cent.x - 2 * k * cent.y + 2 * k * q) + sqrt(dis)) / 4;
            double x2 = (-(-2 * cent.x - 2 * k * cent.y + 2 * k * q) - sqrt(dis)) / 4;
            double y1 = k * x1 + q;
            double y2 = k * x2 + q;
            return {Line(Point(x1, y1), -1.0 / k), Line(Point(x2, y2), -1.0 / k)};
        } else {
            double x1 = focus_1.x;
            double x2 = x1;
            double y1 = focus_1.x + eccentricity();
            double y2 = focus_2.x - eccentricity();
            return {Line(Point(x1, y1), Point(x1 + 1, y1)),
                    Line(Point(x2, y2), Point(x2 + 1, y2))};
        }
    }

    double eccentricity() const { return focus_dist / big_semi_axis; }

    Point center() const {
        return Point((focus_1.x + focus_2.x) / 2, (focus_1.y + focus_2.y) / 2);
    }
};

class Circle : public Ellipse {
private:
    Point circ_center;
    double circ_radius;

public:
    bool operator==(const Shape& another) const override {
        auto circle = dynamic_cast<const Circle*>(&another);
        if ((circle == nullptr) ||
            !((circ_center==circle->circ_center) && equal(circ_radius, circle->circ_radius))) {
            return false;
        }
        return true;
    }

    double radius() const { return circ_radius; }

    double perimeter() const override { return pi * 2 * circ_radius; }

    double area() const override { return pi * pow(circ_radius, 2); }

    void scale(const Point& center, double coefficient) override {
        double new_x = center.x + coefficient * (center.x - center.x);
        double new_y = center.y + coefficient * (center.y - center.y);

        *this = Circle(Point(new_x, new_y), circ_radius * coefficient);
    }

    Circle(const Point& center, double radius)
            : Ellipse(center, center, 2 * radius),
              circ_center(center),
              circ_radius(radius){};
};

class Rectangle : public Polygon {
private:
    std::vector<Point> vertices;
    std::vector<Point> get_verts(const Point& A, const Point& C, double ratio) {
        if (ratio < 1) {
            ratio = 1.0 / ratio;
        }
        double diag = length(A, C);
        double s2 = std::sqrt((diag * diag) / (1.0 + ratio * ratio));

        double vx = C.x - A.x;
        double vy = C.y - A.y;

        double len = std::sqrt(vx * vx + vy * vy);
        vx /= len;
        vy /= len;

        double nx = -vy;
        double ny = vx;

        double dp = dot_product(vx, vy, ny, -nx);
        if (dp < 0) {
            nx = -nx;
            ny = -ny;
        }

        Point B = {A.x + nx * s2, A.y + ny * s2};
        Point M = mid(A, C);
        Point D = {2 * M.x - B.x, 2 * M.y - B.y};

        return {A, B, C, D};
    }

public:
    Rectangle(const Point& A, const Point& C, double x)
            : Polygon(get_verts(A, C, x)) {}

    Point center() {
        return Point((vertices[0].x + vertices[2].x) / 2,
                     (vertices[0].y + vertices[2].y) / 2);
    }

    std::pair<Line, Line> diagonals() {
        return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
    }
};

class Square : public Rectangle {
private:
    std::vector<Point> vertices = getVertices();

public:
    Square(const Point& p_1, const Point& p_2) : Rectangle(p_1, p_2, 1){};

    Circle circumscribedCircle() {
        Point A = vertices[0];
        Point C = vertices[2];
        double rad = length(A, C) / 2;
        return Circle(center(), rad);
    }

    Circle inscribedCircle() {
        Point p_1 = vertices[0];
        Point p_2 = vertices[1];
        double rad = length(p_1, p_2) / 2;
        return Circle(center(), rad);
    }
};

class Triangle : public Polygon {
private:
    std::vector<Point> vertices = getVertices();

    Point point_1 = vertices[0];
    Point point_2 = vertices[1];
    Point point_3 = vertices[2];

    double side_12 = length(point_1, point_2);
    double side_23 = length(point_2, point_3);
    double side_13 = length(point_3, point_1);

public:
    Triangle(const Point& point_1, const Point& point_2, const Point& point_3)
            : Polygon(point_1, point_2, point_3){};

    Circle circumscribedCircle() {
        double rad = (side_23 * side_13 * side_12) / (4 * area());
        Point mid_12 = mid(point_1, point_2);
        Line s_12 = Line(point_1, point_2);
        Point mid_23 = mid(point_2, point_3);
        Line s_23 = Line(point_2, point_3);
        Line p_12 = Line(s_12.coeff_y, -s_12.coeff_x,
                         s_12.coeff_x * mid_12.y - s_12.coeff_y * mid_12.x);
        Line p_23 = Line(s_23.coeff_y, -s_23.coeff_x,
                         s_23.coeff_x * mid_23.y - s_23.coeff_y * mid_23.x);
        Point O = intersection(p_12, p_23);
        return Circle(O, rad);
    }

    Circle inscribedCircle() {
        double rad = (2 * area()) / (side_23 + side_13 + side_12);

        double cent_x =
                (side_23 * point_1.x + side_13 * point_2.x + side_12 * point_3.x) /
                Polygon::perimeter();
        double cent_y =
                (side_23 * point_1.y + side_13 * point_2.y + side_12 * point_3.y) /
                Polygon::perimeter();
        return Circle(Point(cent_x, cent_y), rad);
    }

    Point centroid() const {
        return Point((point_1.x + point_2.x + point_3.x) / 3,
                     (point_1.y + point_2.y + point_3.y) / 3);
    }

    Point orthocenter() {
        Line ab = Line(point_1, point_2);
        Line h_a = Line(-ab.coeff_y, ab.coeff_x,
                        ab.coeff_y * point_3.x - ab.coeff_x * point_3.y);
        Line bc = Line(point_2, point_3);
        Line h_b = Line(-bc.coeff_y, bc.coeff_x,
                        bc.coeff_y * point_1.x - bc.coeff_x * point_1.y);
        return intersection(h_a, h_b);
    }

    Line EulerLine() { return Line(centroid(), orthocenter()); }

    Circle ninePointsCircle() {
        return Triangle(mid(point_1, point_2), mid(point_2, point_3),
                        mid(point_3, point_1))
                .circumscribedCircle();
    }
};
