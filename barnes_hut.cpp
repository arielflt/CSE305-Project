#include "barnes_hut.hpp"
#include <iostream>
#include <stack>
#include <cmath>

void barnes_hut(Scenario &bodies, double time_step, double total_time) {
    for (double t = 0; t < total_time; t += time_step) {
        barnes_hut_update_step(bodies, time_step);

        // Debug print positions, velocities, and forces
        std::cout << "Time: " << t + time_step << std::endl;
        for (size_t i = 0; i < bodies.r.size(); ++i) {
            std::cout << "Body " << i + 1 << ": Position (" << bodies.r[i].x << ", " << bodies.r[i].y << "), Velocity (" << bodies.v[i].x << ", " << bodies.v[i].y << "), Force (" << bodies.f[i].x << ", " << bodies.f[i].y << ")\n";
        }
    }
}

void barnes_hut_update_step(Scenario &bodies, double time_step) {
    QuadNode *root = QuadNode::constructBarnesHutTree(&bodies);

    // Initialize forces to zero
    bodies.f.assign(bodies.r.size(), Vector2D{0.0, 0.0});

    /* Calculate the force exerted */
    for (size_t i = 0; i < bodies.r.size(); i++) {
        const double m = bodies.m[i];
        const Vector2D &r = bodies.r[i];

        auto update_v = [&](const Vector2D &other_r, const double other_m) {
            Vector2D dr = other_r - r;
            double dist_sq = std::max(dr.norm2(), 1e-6);  // Avoid division by zero
            double dist = std::sqrt(dist_sq);
            if (dist > 1e-6) {  // Avoid very small distances causing large forces
                double force_mag = G * other_m * m / dist_sq;
                Vector2D force = dr * (force_mag / dist);
                bodies.f[i] += force;  // Store the force
                bodies.v[i] += force * (time_step / m);
            }
        };

        std::stack<QuadNode *> stack;
        stack.push(root);
        while (!stack.empty()) {
            QuadNode *curr = stack.top();
            stack.pop();

            if (!curr->body_id.empty()) {
                for (size_t curr_body : curr->body_id) {
                    if (curr_body != i) {
                        update_v(bodies.r[curr_body], bodies.m[curr_body]);
                    }
                }
            } else if (curr->isFarEnough(bodies.r[i])) {
                update_v(curr->center_of_mass, curr->m);
            } else {
                for (int j = 0; j < 4; j++) {
                    if (curr->children[j]) stack.push(curr->children[j]);
                }
            }
        }
    }

    /* Update positions */
    for (size_t i = 0; i < bodies.r.size(); i++) {
        bodies.r[i] += bodies.v[i] * time_step;
    }
    delete root;
}

void QuadNode::addBody(int index) {
    if (!isInside(scenario->r[index])) {
        return;
    }

    // If node is empty, add body to node
    if (is_empty || dimension.x < 1e-3) {
        is_empty = false;
        body_id.push_back(index);
        updateCenterOfMass(index);
        return;
    }

    // If there is only one body in the node, create a new define the sub
    // node of the old because the node was not split yet. The case where
    // there are multiple bodies is handled above, so there should always be
    // only one body here.
    if (!body_id.empty()) {
        quad quad_old = getQuad(scenario->r[body_id[0]]);
        Vector2D quad_center = getQuadCenter(quad_old);
        children[quad_old] = new QuadNode(scenario, quad_center, dimension / 2);
        children[quad_old]->addBody(body_id[0]);
        body_id.clear();
    }

    // Get quad where new body belongs
    quad quad_new = getQuad(scenario->r[index]);

    // If the new node is empty, initialize the node
    if (children[quad_new] == nullptr) {
        Vector2D quad_center = getQuadCenter(quad_new);
        children[quad_new] = new QuadNode(scenario, quad_center, dimension / 2);
    }
    // Add the body to the node and update center of mass
    children[quad_new]->addBody(index);
    updateCenterOfMass(index);
}
