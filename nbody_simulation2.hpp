#ifndef NBODY_SIMULATION2_HPP
#define NBODY_SIMULATION2_HPP

#include <iostream>
#include <vector>
#include <Magick++.h>

struct Vector2D {
    double x, y;

    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2D operator-(const Vector2D& other) const {
        return {x - other.x, y - other.y};
    }

    Vector2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
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
};

struct Scenario {
    std::vector<double> m;
    std::vector<Vector2D> r;
    std::vector<Vector2D> v;
    std::vector<Vector2D> f;
};

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time);
void draw_arrow(Magick::Image &frame, int x1, int y1, double dx, double dy, const std::string &color);
void save_frame(const std::vector<Vector2D> &positions, const std::vector<Vector2D> &velocities, const std::vector<Vector2D> &forces, int n, int t, std::vector<Magick::Image> &frames, double min_x, double max_x, double min_y, double max_y);
void visualize(const std::vector<std::vector<Vector2D>> &all_positions, const std::vector<std::vector<Vector2D>> &all_velocities, const std::vector<std::vector<Vector2D>> &all_forces, int n, double time_step, double total_time);

#endif // NBODY_SIMULATION2_HPP
