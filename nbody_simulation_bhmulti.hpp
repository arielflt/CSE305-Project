#ifndef NBODY_SIMULATION_BHMULTI_HPP
#define NBODY_SIMULATION_BHMULTI_HPP

#include <iostream>
#include <vector>
#include <Magick++.h>
#include <cmath>

struct Vector2D {
    double x, y;

    Vector2D() : x(0), y(0) {}  // Default constructor
    Vector2D(double x_val, double y_val) : x(x_val), y(y_val) {}  // Parameterized constructor


    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2D operator-(const Vector2D& other) const {
        return {x - other.x, y - other.y};
    }

    Vector2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    Vector2D operator/(double scalar) const {
        return {x / scalar, y / scalar};
    }

    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2D& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2D& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    double norm2() const {
        return x * x + y * y;
    }
};

struct Scenario {
    std::vector<double> m;
    std::vector<Vector2D> r;
    std::vector<Vector2D> v;
    std::vector<Vector2D> f;
};

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time, int &num_threads);
void draw_arrow(Magick::Image &frame, int x1, int y1, double dx, double dy, const std::string &color);
void save_frame(const std::vector<Vector2D> &positions, const std::vector<Vector2D> &velocities, const std::vector<Vector2D> &forces, int n, int t, std::vector<Magick::Image> &frames, double min_x, double max_x, double min_y, double max_y);
void visualize(const std::vector<std::vector<Vector2D>> &all_positions, const std::vector<std::vector<Vector2D>> &all_velocities, const std::vector<std::vector<Vector2D>> &all_forces, int n, double time_step, double total_time);

#endif // NBODY_SIMULATION_BHMULTI_HPP
