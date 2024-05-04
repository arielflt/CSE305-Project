#ifndef NBODY_SIMULATION_HPP
#define NBODY_SIMULATION_HPP

#include <vector>
#include <thread>

// Define a struct for 2D vector to store position and velocity
struct Vector2D {
    double x, y;
};

// Function prototypes
void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time);
void parallel_compute_forces(int n, const std::vector<double>& masses, const std::vector<Vector2D>& positions, std::vector<Vector2D>& forces, double G, int thread_count);
void parallel_update(int n, std::vector<double>& masses, std::vector<Vector2D>& positions, std::vector<Vector2D>& velocities, std::vector<Vector2D>& forces, double time_step, int thread_count);
void log_state(int step, double time, const std::vector<double>& masses, const std::vector<Vector2D>& positions, const std::vector<Vector2D>& velocities, const std::vector<Vector2D>& forces);

#endif // NBODY_SIMULATION_HPP
