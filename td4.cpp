
#include <cfloat>
#include <climits>
#include <thread>
#include <numeric>
#include <iterator>
#include <vector>
#include <mutex>
#include <iostream>



def gather_input():
    # Number of bodies
    n = int(input("Enter the number of bodies to simulate: "))

    # Initializing lists for masses, positions, and velocities
    masses = []
    positions = []
    velocities = []

    # Collecting data for each body
    for i in range(n):
        print(f"Enter data for body {i+1}:")
        mass = float(input("  Mass (kg): "))
        x_pos = float(input("  Initial x position (meters): "))
        y_pos = float(input("  Initial y position (meters): "))
        x_vel = float(input("  Initial x velocity (m/s): "))
        y_vel = float(input("  Initial y velocity (m/s): "))
        
        # Append each to the corresponding list
        masses.append(mass)
        positions.append((x_pos, y_pos))
        velocities.append((x_vel, y_vel))

    # Simulation time parameters
    time_step = float(input("Enter the time step for the simulation (seconds): "))
    total_time = float(input("Enter the total simulation time (seconds): "))

    return n, masses, positions, velocities, time_step, total_time

# Example usage
n_bodies, masses, positions, velocities, time_step, total_time = gather_input()
