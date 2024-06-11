#ifndef TEST_HPP
#define TEST_HPP

#include "nbody_simulation.hpp"
#include "barnes_hut.hpp"
#include <chrono>
#include <iostream>
#include <vector>

void setup_solar_system(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities);
void run_simulation(const std::string &algorithm_name, void (*algorithm)(Scenario &, double, double), Scenario &bodies, double time_step, double total_time);
void simple_nbody_algorithm(Scenario &bodies, double time_step, double total_time);

#endif // TEST_HPP
