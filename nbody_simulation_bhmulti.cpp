#include "nbody_simulation_bhmulti.hpp"
#include "barnes_hut_multi.hpp"
#include <iostream>
#include <vector>
#include <Magick++.h>
#include <cmath>
#include <mutex>

using namespace Magick;

std::mutex force_mutex;

void gather_input(int &n, std::vector<double> &masses, std::vector<Vector2D> &positions, std::vector<Vector2D> &velocities, double &time_step, double &total_time, int &num_threads) {
    std::cout << "Enter the number of bodies to simulate: ";
    std::cin >> n;

    masses.resize(n);
    positions.resize(n);
    velocities.resize(n);

    for (int i = 0; i < n; ++i) {
        std::cout << "Enter data for body " << (i + 1) << ":\n";
        std::cout << "  Mass (kg): ";
        std::cin >> masses[i];
        while (masses[i] <= 0) {
            std::cout << "  Incorrect Mass. Enter again Mass (kg): ";
            std::cin >> masses[i];
        }
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
    std::cout << "Enter the number of threads to use: ";
    std::cin >> num_threads;
}

void draw_arrow(Magick::Image &frame, int x1, int y1, double dx, double dy, const std::string &color) {
    double angle = std::atan2(dy, dx);
    const double arrow_length = 15;
    const double arrow_angle = M_PI / 6; // 30 degrees for each arrowhead wing

    int x2 = x1 + static_cast<int>(arrow_length * std::cos(angle));
    int y2 = y1 + static_cast<int>(arrow_length * std::sin(angle));

    int x3 = x2 - static_cast<int>(arrow_length / 3 * std::cos(angle - arrow_angle));
    int y3 = y2 - static_cast<int>(arrow_length / 3 * std::sin(angle - arrow_angle));
    int x4 = x2 - static_cast<int>(arrow_length / 3 * std::cos(angle + arrow_angle));
    int y4 = y2 - static_cast<int>(arrow_length / 3 * std::sin(angle + arrow_angle));

    frame.strokeColor(color);

    frame.draw(Magick::DrawableLine(x1, y1, x2, y2));
    frame.draw(Magick::DrawableLine(x2, y2, x3, y3));
    frame.draw(Magick::DrawableLine(x2, y2, x4, y4));
}

void save_frame(const std::vector<Vector2D> &positions, const std::vector<Vector2D> &velocities, const std::vector<Vector2D> &forces, int n, int t, std::vector<Magick::Image> &frames, double min_x, double max_x, double min_y, double max_y) {
    int width = 800;
    int height = 800;
    Magick::Image frame(Magick::Geometry(width, height), "white");
    frame.strokeWidth(2);
    std::vector<std::string> colors = {"red", "green", "blue", "yellow", "cyan", "magenta", "orange", "purple", "brown", "pink"};

    double range_x = max_x - min_x;
    double range_y = max_y - min_y;

    for (int i = 0; i < n; ++i) {
        if (i >= positions.size() || i >= velocities.size() || i >= forces.size()) {
            std::cerr << "Error: Out of bounds access in save_frame\n";
            return;
        }

        int x = ((positions[i].x - min_x) / range_x) * width;
        int y = height - ((positions[i].y - min_y) / range_y) * height;

        frame.fillColor(colors[i % colors.size()]);
        frame.strokeColor("black");
        frame.draw(Magick::DrawableCircle(x, y, x + 3, y + 3));

        double vx = (velocities[i].x / range_x) * width;
        double vy = -(velocities[i].y / range_y) * height; // Negative because the y-axis is inverted in the image
        double Fx = (forces[i].x / range_x) * width;
        double Fy = -(forces[i].y / range_y) * height; // Negative because the y-axis is inverted in the image

        frame.fillColor("none"); // Reset fill color before drawing arrows
        draw_arrow(frame, x, y, vx, vy, colors[i % colors.size()]); // velocities are in the color of the object
        if (n > 1) {
            draw_arrow(frame, x, y, Fx, Fy, "black"); // forces are in color black
        }
    }

    frame.strokeColor("black");
    std::string border_info = "Time: " + std::to_string(t) +
                              "\nRange: [" + std::to_string(min_x) + ", " + std::to_string(max_x) + "] x " +
                              "[" + std::to_string(min_y) + ", " + std::to_string(max_y) + "]";
    frame.annotate(border_info, Magick::NorthWestGravity);
    frames.push_back(frame);
}


void visualize(const std::vector<std::vector<Vector2D>> &all_positions, const std::vector<std::vector<Vector2D>> &all_velocities, const std::vector<std::vector<Vector2D>> &all_forces, int n, double time_step, double total_time) {
    Magick::InitializeMagick(nullptr);
    std::vector<Magick::Image> frames;

    std::cout << "Initializing visualization...\n";

    if (all_positions.empty() || all_positions[0].empty()) {
        std::cerr << "Error: No positions available for visualization\n";
        return;
    }

    double min_x = all_positions[0][0].x;
    double max_x = all_positions[0][0].x;
    double min_y = all_positions[0][0].y;
    double max_y = all_positions[0][0].y;

    for (const auto &positions_at_time : all_positions) {
        for (const auto &pos : positions_at_time) {
            if (pos.x < min_x) min_x = pos.x;
            if (pos.x > max_x) max_x = pos.x;
            if (pos.y < min_y) min_y = pos.y;
            if (pos.y > max_y) max_y = pos.y;
        }
    }

    double margin_x = (max_x - min_x) * 0.1;
    double margin_y = (max_y - min_y) * 0.1;
    min_x -= margin_x;
    max_x += margin_x;
    min_y -= margin_y;
    max_y += margin_y;

    std::cout << "Generating frames...\n";

    for (size_t t = 0; t < all_positions.size(); ++t) {
        std::cout << "Saving frame " << t << "...\n";
        if (t >= all_velocities.size() || t >= all_forces.size()) {
            std::cerr << "Error: Out of bounds access in visualize\n";
            return;
        }

        // Ensure the sizes of the positions, velocities, and forces vectors match
        if (all_positions[t].size() != all_velocities[t].size() || all_positions[t].size() != all_forces[t].size()) {
            std::cerr << "Error: Mismatch in sizes of positions, velocities, and forces at time " << t << "\n";
            return;
        }

        save_frame(all_positions[t], all_velocities[t], all_forces[t], n, t * time_step, frames, min_x, max_x, min_y, max_y);
    }

    std::cout << "Writing images...\n";
    Magick::writeImages(frames.begin(), frames.end(), "nbody_simulation3.gif");
    std::cout << "Images written.\n";
}



int main() {
    int n;
    std::vector<double> masses;
    std::vector<Vector2D> positions, velocities;
    double time_step, total_time;
    int num_threads;

    gather_input(n, masses, positions, velocities, time_step, total_time, num_threads);

    Scenario bodies;
    bodies.m = masses;
    bodies.r = positions;
    bodies.v = velocities;
    bodies.f = std::vector<Vector2D>(n, Vector2D{0, 0});  // Initialize forces vector

    std::vector<Vector2D> forces(n, Vector2D{0, 0});

    std::vector<std::vector<Vector2D>> all_positions, all_velocities, all_forces;
    all_positions.push_back(bodies.r);
    all_velocities.push_back(bodies.v);
    all_forces.push_back(bodies.f);

    std::cout << "Starting simulation...\n";
    barnes_hut(bodies, time_step, total_time, all_positions, all_velocities, all_forces, num_threads);
    std::cout << "Simulation complete.\n";

    std::cout << "Starting visualization...\n";
    visualize(all_positions, all_velocities, all_forces, n, time_step, total_time);
    std::cout << "Visualization complete.\n";

    return 0;
}



