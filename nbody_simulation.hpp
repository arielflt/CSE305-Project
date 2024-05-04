#ifndef NBODY_SIMULATION_HPP
#define NBODY_SIMULATION_HPP

#include <vector>

struct Vector2D {
    double x, y;
};

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time);
void compute_forces(const int n, const std::vector<double>& masses, const std::vector<Vector2D>& positions, std::vector<Vector2D>& forces, const double G = 6.67430e-11);
void update_bodies(int n, std::vector<double>& masses, std::vector<Vector2D>& positions, std::vector<Vector2D>& velocities, std::vector<Vector2D>& forces, double time_step);

#endif // NBODY_SIMULATION_HPP
