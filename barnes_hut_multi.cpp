#include "barnes_hut_multi.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <stack>
#include <thread>


void barnes_hut_update_step_aux(int start, int end, Scenario &bodies, QuadNode *root) {
    for (int i = start; i < end; ++i) {
        const double m = bodies.m[i];
        const Vector2D &r = bodies.r[i];

        auto update_v = [&](const Vector2D &other_r, const double other_m) {
            Vector2D dr = other_r - r;
            double dist_sq = std::max(dr.norm2(), 1e-6);
            double dist = sqrt(dist_sq);
            double force_mag = G * other_m * m / dist_sq;
            Vector2D force = dr * (force_mag / dist);
            bodies.v[i] += force * (time_step / m);
        };

        std::stack<QuadNode *> stack;
        stack.push(root);
        while (!stack.empty()) {
            QuadNode *curr = stack.top();
            stack.pop();

            if (!curr->body_id.empty()) {
                for (int curr_body : curr->body_id) {
                    if (curr_body != i) {
                        update_v(bodies.r[curr_body], bodies.m[curr_body]);
                    }
                }
            } else if (curr->isFarEnough(bodies.r[i])) {
                update_v(curr->center_of_mass, curr->m);
            } else {
                for (int j = 0; j < 4; ++j) {
                    if (curr->children[j]) stack.push(curr->children[j]);
                }
            }
        }
    }
}
void barnes_hut_update_step_multi(Scenario &bodies, int num_threads) {
    QuadNode *root = QuadNode::constructBarnesHutTree(&bodies);
    std::vector<std::thread> threads;
    int num_bodies = bodies.r.size();
    int num_bodies_per_thread = num_bodies / num_threads;

    for (int i = 0; i < num_threads - 1; ++i) {
        threads.emplace_back(barnes_hut_update_step_aux, i * num_bodies_per_thread, (i + 1) * num_bodies_per_thread, std::ref(bodies), root);
    }
    barnes_hut_update_step_aux((num_threads - 1) * num_bodies_per_thread, num_bodies, bodies, root);

    for (auto &thread : threads) {
        thread.join();
    }

    // Update positions
    for (size_t i = 0; i < bodies.r.size(); ++i) {
        bodies.r[i] += bodies.v[i] * time_step;
    }

    delete root;
}


void barnes_hut(Scenario &bodies, double time_step, double total_time, std::vector<std::vector<Vector2D>> &all_positions, std::vector<std::vector<Vector2D>> &all_velocities, std::vector<std::vector<Vector2D>> &all_forces, int num_threads) {
    for (double t = 0; t < total_time; t += time_step) {
        barnes_hut_update_step_multi(bodies, num_threads);

        // Store positions, velocities, and forces for each body
        all_positions.push_back(bodies.r);
        all_velocities.push_back(bodies.v);
        all_forces.push_back(bodies.f);

        // Debug print positions, velocities, and forces
        std::cout << "Time: " << t + time_step << std::endl;
        for (size_t i = 0; i < bodies.r.size(); ++i) {
            std::cout << "Body " << i + 1 << ": Position (" << bodies.r[i].x << ", " << bodies.r[i].y << "), Velocity (" << bodies.v[i].x << ", " << bodies.v[i].y << "), Force (" << bodies.f[i].x << ", " << bodies.f[i].y << ")\n";
        }
    }
}