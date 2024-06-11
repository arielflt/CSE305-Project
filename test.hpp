#ifndef TEST_HPP
#define TEST_HPP

#include "nbody_simulation.hpp"
#include "barnes_hut.hpp"
#include <chrono>
#include <vector>
#include <iostream>

void setup_solar_system(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities);

void run_simulation(const std::string &algorithm_name, void (*algorithm)(Scenario &, double, double), Scenario &bodies, double time_step, double total_time);

#endif // TEST_HPP
