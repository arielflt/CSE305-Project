#include "nbody_simulation_p.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <thread>

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

void parallel_compute_forces(int n, const std::vector<double>& masses, const std::vector<Vector2D>& positions, std::vector<Vector2D>& forces, double G, int thread_count) {
    std::vector<std::thread> threads;
    forces.assign(n, Vector2D{0, 0});

    auto compute_chunk = [n, &masses, &positions, &forces, G](int start, int end) {
        for (int i = start; i < end; ++i) {
            for (int j = i + 1; j < n; ++j) {
                Vector2D delta = {positions[j].x - positions[i].x, positions[j].y - positions[i].y};
                double dist_squared = delta.x * delta.x + delta.y * delta.y;
                double dist = std::sqrt(dist_squared);
                double force_magnitude = G * masses[i] * masses[j] / dist_squared;
                Vector2D force_ij = {force_magnitude * delta.x / dist, force_magnitude * delta.y / dist};
                forces[i].x += force_ij.x;
                forces[i].y += force_ij.y;
                forces[j].x -= force_ij.x;
                forces[j].y -= force_ij.y;
            }
        }
    };

    int chunk_size = n / thread_count;
    for (int i = 0; i < thread_count; ++i) {
        int start = i * chunk_size;
        int end = (i == thread_count - 1) ? n : (i + 1) * chunk_size;
        threads.push_back(std::thread(compute_chunk, start, end));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void parallel_update(int n, std::vector<double>& masses, std::vector<Vector2D>& positions, std::vector<Vector2D>& velocities, std::vector<Vector2D>& forces, double time_step, int thread_count) {
    std::vector<std::thread> threads;

    auto update_chunk = [&masses, &positions, &velocities, &forces, time_step](int start, int end) {
        for (int i = start; i < end; ++i) {
            velocities[i].x += forces[i].x / masses[i] * time_step;
            velocities[i].y += forces[i].y / masses[i] * time_step;
            positions[i].x += velocities[i].x * time_step;
            positions[i].y += velocities[i].y * time_step;
        }
    };

    int chunk_size = n / thread_count;
    for (int i = 0; i < thread_count; ++i) {
        int start = i * chunk_size;
        int end = (i == thread_count - 1) ? n : (i + 1) * chunk_size;
        threads.push_back(std::thread(update_chunk, start, end));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void log_state(int step, double time, const std::vector<double>& masses, const std::vector<Vector2D>& positions, const std::vector<Vector2D>& velocities, const std::vector<Vector2D>& forces) {
    std::cout << "Time: " << time << std::endl;
    for (int i = 0; i < positions.size(); ++i) {
        std::cout << "Body " << i + 1 << ": Mass = " << masses[i]
                  << " Position = (" << positions[i].x << ", " << positions[i].y << ")"
                  << " Velocity = (" << velocities[i].x << ", " << velocities[i].y << ")"
                  << " Force = (" << forces[i].x << ", " << forces[i].y << ")" << std::endl;
    }
}

int main() {
    int n;
    std::vector<double> masses;
    std::vector<Vector2D> positions, velocities;
    double time_step, total_time;

    gather_input(n, masses, positions, velocities, time_step, total_time);

    int thread_count = std::thread::hardware_concurrency(); // Utilize all available hardware threads
    std::vector<Vector2D> forces(n);

    for (double t = 0; t < total_time; t += time_step) {
        parallel_compute_forces(n, masses, positions, forces, 6.67430e-11, thread_count);
        parallel_update(n, masses, positions, velocities, forces, time_step, thread_count);

        std::cout << "Time: " << t + time_step << std::endl;
        for (int i = 0; i < n; ++i) {
            std::cout << "Body " << i + 1 << ": Position (" << positions[i].x << ", " << positions[i].y << ")" << std::endl;
        }
    }

    for (double t = 0; t < total_time; t += time_step) {
        parallel_compute_forces(n, masses, positions, forces, 6.67430e-11, thread_count);
        parallel_update(n, masses, positions, velocities, forces, time_step, thread_count);

        log_state(t / time_step + 1, t + time_step, masses, positions, velocities, forces);
    }

    return 0;
}
