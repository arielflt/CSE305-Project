#ifndef NBODY_SIMULATION2_HPP
#define NBODY_SIMULATION2_HPP

#include <vector>

struct Vector2D {
    double x, y;

    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    Vector2D operator/(double scalar) const {
        return {x / scalar, y / scalar};
    }

    Vector2D operator-(const Vector2D& other) const {
        return {x - other.x, y - other.y};
    }

    double norm2() const {
        return x * x + y * y;
    }
};

struct Scenario {
    std::vector<Vector2D> r;
    std::vector<Vector2D> v;
    std::vector<double> m;
};

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time);
void barnes_hut(Scenario &bodies, double time_step, double total_time);

#endif // NBODY_SIMULATION2_HPP
