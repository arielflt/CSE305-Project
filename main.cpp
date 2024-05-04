#include <iostream>
#include <vector>
#include <cmath>
#include "nbody_simulation.hpp"

int main() {
    int n;
    std::vector<double> masses;
    std::vector<Vector2D> positions, velocities;
    double time_step, total_time;

    gather_input(n, masses, positions, velocities, time_step, total_time);

    std::vector<Vector2D> forces(n);

    for (double t = 0; t < total_time; t += time_step) {
        compute_forces(n, masses, positions, forces);
        update_bodies(n, masses, positions, velocities, forces, time_step);
        // Optional: Output positions or other details here for visualization or debugging
    }

    return 0;
}