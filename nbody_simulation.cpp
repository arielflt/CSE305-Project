#include "nbody_simulation.hpp"
#include <iostream>
#include <cmath>
#include <vector>

// Function definitions should just implement the declared functions in the header file
void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time) {
    std::cout << "Enter the number of bodies to simulate: ";
    std::cin >> n;

    masses.resize(n);
    positions.resize(n);
    velocities.resize(n);

    for (int i = 0; i < n; ++i) {
        std::cout << "Enter data for body " << (i + 1) << ":\n";
        std::cout << "  Mass (kg): ";
        std::cin >> masses[i];
        std::cout << "  Initial x position (meters): ";
        std::cin >> positions[i].x;
        std::cout << "  Initial y position (meters): ";
        std::cin >> positions[i].y;
        std::cout << "  Initial x velocity (m/s): ";
        std::cin >> velocities[i].x;
        std::cout << "  Initial y velocity (m/s): ";
        std::cin >> velocities[i].y;
    }

    std::cout << "Enter the time step for the simulation (seconds): ";
    std::cin >> time_step;
    std::cout << "Enter the total simulation time (seconds): ";
    std::cin >> total_time;
}

void compute_forces(const int n, const std::vector<double>& masses, const std::vector<Vector2D>& positions, std::vector<Vector2D>& forces, double G) {
    forces.assign(n, Vector2D{0, 0});

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                Vector2D delta = {positions[j].x - positions[i].x, positions[j].y - positions[i].y};
                double dist_squared = delta.x * delta.x + delta.y * delta.y;
                double dist = std::sqrt(dist_squared);
                double force_magnitude = G * masses[i] * masses[j] / dist_squared;
                forces[i].x += force_magnitude * delta.x / dist;
                forces[i].y += force_magnitude * delta.y / dist;
            }
        }
    }
}

void update_bodies(int n, std::vector<double>& masses, std::vector<Vector2D>& positions, std::vector<Vector2D>& velocities, std::vector<Vector2D>& forces, double time_step) {
    for (int i = 0; i < n; ++i) {
        velocities[i].x += forces[i].x / masses[i] * time_step;
        velocities[i].y += forces[i].y / masses[i] * time_step;
        positions[i].x += velocities[i].x * time_step;
        positions[i].y += velocities[i].y * time_step;
    }
}

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

        std::cout << "Time: " << t + time_step << std::endl;
        for (int i = 0; i < n; ++i) {
            std::cout << "Body " << i + 1 << ": Position (" << positions[i].x << ", " << positions[i].y << ")" << std::endl;
        }
    }

    return 0;
}
