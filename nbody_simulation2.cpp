#include "nbody_simulation2.hpp"
#include "barnes_hut.hpp"
#include <iostream>
#include <vector>

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

int main() {
    int n;
    std::vector<double> masses;
    std::vector<Vector2D> positions, velocities;
    double time_step, total_time;

    gather_input(n, masses, positions, velocities, time_step, total_time);

    Scenario bodies;
    bodies.m = masses;
    bodies.r = positions;
    bodies.v = velocities;

    barnes_hut(bodies, time_step, total_time);

    std::cout << "Final positions and velocities:\n";
    for (size_t i = 0; i < bodies.r.size(); ++i) {
        std::cout << "Body " << i + 1 << ": Position (" << bodies.r[i].x << ", " << bodies.r[i].y << "), Velocity (" << bodies.v[i].x << ", " << bodies.v[i].y << ")\n";
    }

    return 0;
}
