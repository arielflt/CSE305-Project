#ifndef NBODY_SIMULATION_HPP
#define NBODY_SIMULATION_HPP

#include <vector>
#include <Magick++.h>

struct Vector2D {
    double x, y;
};

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time);
void compute_forces(const int n, const std::vector<double>& masses, const std::vector<Vector2D>& positions, std::vector<Vector2D>& forces, const double G = 6.67430e-11);
void update_bodies(int n, std::vector<double>& masses, std::vector<Vector2D>& positions, std::vector<Vector2D>& velocities, std::vector<Vector2D>& forces, double time_step);
void draw_arrow(Magick::Image &frame, int x1, int y1, double dx, double dy, const std::string &color);
void save_frame(const std::vector<Vector2D>& positions, const std::vector<Vector2D>& velocities, const std::vector<Vector2D>& forces, int n, int t, std::vector<Magick::Image>& frames, double min_x, double max_x, double min_y, double max_y);
void visualize(const std::vector<std::vector<Vector2D>>& all_positions, const std::vector<std::vector<Vector2D>>& all_velocities, const std::vector<std::vector<Vector2D>>& all_forces, int n, double time_step, double total_time);
#endif // NBODY_SIMULATION_HPP
