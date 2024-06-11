#include "test.hpp"

void setup_solar_system(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities) {
    n = 9;
    masses = {
        1.9885e30, // Sun
        3.3011e23, // Mercury
        4.8675e24, // Venus
        5.9724e24, // Earth
        6.4171e23, // Mars
        1.8982e27, // Jupiter
        5.6834e26, // Saturn
        8.6810e25, // Uranus
        1.0241e26  // Neptune
    };

    positions = {
        {0, 0}, // Sun
        {5.7e10, 0}, // Mercury
        {1.08e11, 0}, // Venus
        {1.496e11, 0}, // Earth
        {2.279e11, 0}, // Mars
        {7.785e11, 0}, // Jupiter
        {1.429e12, 0}, // Saturn
        {2.871e12, 0}, // Uranus
        {4.498e12, 0}  // Neptune
    };

    velocities = {
        {0, 0}, // Sun
        {0, 4.74e4}, // Mercury
        {0, 3.5e4}, // Venus
        {0, 2.98e4}, // Earth
        {0, 2.41e4}, // Mars
        {0, 1.31e4}, // Jupiter
        {0, 9.7e3}, // Saturn
        {0, 6.8e3}, // Uranus
        {0, 5.43e3}  // Neptune
    };
}

void run_simulation(const std::string &algorithm_name, void (*algorithm)(Scenario &, double, double), Scenario &bodies, double time_step, double total_time) {
    auto start = std::chrono::high_resolution_clock::now();
    algorithm(bodies, time_step, total_time);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Algorithm: " << algorithm_name << "\n";
    std::cout << "Simulation Time: " << duration.count() << " seconds\n";
    std::cout << "Final positions:\n";
    for (size_t i = 0; i < bodies.r.size(); ++i) {
        std::cout << "Body " << i + 1 << ": (" << bodies.r[i].x << ", " << bodies.r[i].y << ")\n";
    }
}

void simple_nbody_algorithm(Scenario &bodies, double time_step, double total_time) {
    std::vector<Vector2D> forces(bodies.r.size());
    for (double t = 0; t < total_time; t += time_step) {
        compute_forces(bodies.r.size(), bodies.m, bodies.r, forces);
        update_bodies(bodies.r.size(), bodies.m, bodies.r, bodies.v, forces, time_step);
    }
}

int main() {
    int n;
    std::vector<double> masses;
    std::vector<Vector2D> positions, velocities;
    double time_step = 3600; // One hour time step
    double total_time = 86400 * 365; // One year simulation

    setup_solar_system(n, masses, positions, velocities);

    Scenario bodies_simple;
    bodies_simple.m = masses;
    bodies_simple.r = positions;
    bodies_simple.v = velocities;

    Scenario bodies_barnes_hut = bodies_simple;

    run_simulation("Simple N-Body Algorithm", simple_nbody_algorithm, bodies_simple, time_step, total_time);
    run_simulation("Barnes-Hut Algorithm", barnes_hut, bodies_barnes_hut, time_step, total_time);

    return 0;
}
