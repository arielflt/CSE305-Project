#include "barnes_hut_multi.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <stack>
#include <thread>

const double theta = 0.5; // Threshold for the approximation
const double G = 6.67430e-11; // Gravitational constant

const double canvas_width = 1000.0;
const double canvas_height = 1000.0;
const double universe_width = 1000.0;
const double universe_height = 1000.0;


QuadNode::QuadNode(Scenario *const bodies, const Vector2D &center, const Vector2D &dimension)
    : center(center), dimension(dimension), scenario(bodies), center_of_mass(center) {}

QuadNode::~QuadNode() {
    for (int i = 0; i < 4; i++) delete children[i];
}

void QuadNode::addBody(int index) {
    if (!isInside(scenario->r[index])) return;

    if (is_empty) {
        body_id.push_back(index);
        updateCenterOfMass(index);
        is_empty = false;
        return;
    }

    if (body_id.size() == 1) {
        int existing_body = body_id[0];
        body_id.clear();

        quad q = getQuad(scenario->r[existing_body]);
        if (!children[q]) {
            children[q] = new QuadNode(scenario, getQuadCenter(q), dimension / 2);
        }
        children[q]->addBody(existing_body);
    }

    quad q = getQuad(scenario->r[index]);
    if (!children[q]) {
        children[q] = new QuadNode(scenario, getQuadCenter(q), dimension / 2);
    }
    children[q]->addBody(index);

    updateCenterOfMass(index);
}


QuadNode* QuadNode::constructBarnesHutTree(Scenario *bodies) {
    std::cout << "Initializing root node.\n";
    QuadNode *root = new QuadNode(bodies, Vector2D(canvas_width / 2., canvas_height / 2.), Vector2D(universe_width, universe_height));

    for (size_t i = 0; i < bodies->r.size(); i++) {
        std::cout << "Adding body " << i << "\n";
        root->addBody(i);
    }

    std::cout << "Tree construction complete.\n";
    return root;
}

bool QuadNode::isFarEnough(const Vector2D &point) const {
    Vector2D dr = center_of_mass - point;
    double dist_sq = std::max(dr.norm2(), 1e-6);
    return dimension.x * dimension.x / dist_sq < theta * theta;
}

QuadNode::quad QuadNode::getQuad(const Vector2D &r) const {
    return r.x < center.x ? (r.y < center.y ? sw : nw) : (r.y < center.y ? se : ne);
}

void QuadNode::updateCenterOfMass(size_t id) {
    double new_m = m + scenario->m[id];
    center_of_mass = (center_of_mass * m + scenario->r[id] * scenario->m[id]) / new_m;
    m = new_m;
}

Vector2D QuadNode::getQuadCenter(quad q) const {
    switch (q) {
        case nw:
            return center + Vector2D(-dimension.x / 4, dimension.y / 4);
        case ne:
            return center + Vector2D(dimension.x / 4, dimension.y / 4);
        case sw:
            return center + Vector2D(-dimension.x / 4, -dimension.y / 4);
        case se:
            return center + Vector2D(dimension.x / 4, -dimension.y / 4);
        default:
            return Vector2D(0, 0);
    }
}

bool QuadNode::isInside(const Vector2D &point) const {
    return point.x <= center.x + dimension.x / 2 && point.x >= center.x - dimension.x / 2 &&
           point.y <= center.y + dimension.y / 2 && point.y >= center.y - dimension.y / 2;
}

void barnes_hut_update_step_aux(int start, int end, Scenario &bodies, QuadNode *root, double time_step) {
    std::cout << "Auxiliary update step for range " << start << " to " << end << std::endl;
    for (int i = start; i < end; ++i) {
        if (i >= bodies.m.size() || i >= bodies.r.size() || i >= bodies.v.size() || i >= bodies.f.size()) {
            std::cerr << "Error: Out of bounds access during force calculation\n";
            return;
        }

        const double m = bodies.m[i];
        const Vector2D &r = bodies.r[i];

        std::cout << "Updating body " << i << " at position (" << r.x << ", " << r.y << ")\n";

        auto update_v = [&](const Vector2D &other_r, const double other_m) {
            Vector2D dr = other_r - r;
            double dist_sq = std::max(dr.norm2(), 1e-6);
            double dist = sqrt(dist_sq);
            double force_mag = G * other_m * m / dist_sq;
            Vector2D force = dr * (force_mag / dist);
            bodies.v[i] += force * (time_step / m);
            bodies.f[i] += force;  // Update forces
        };

        std::stack<QuadNode *> stack;
        stack.push(root);
        while (!stack.empty()) {
            QuadNode *curr = stack.top();
            stack.pop();

            if (!curr->body_id.empty()) {
                for (int curr_body : curr->body_id) {
                    if (curr_body != i) {
                        if (curr_body >= bodies.r.size() || curr_body >= bodies.m.size()) {
                            std::cerr << "Error: Out of bounds access during stack processing\n";
                            return;
                        }
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
    std::cout << "Auxiliary update step complete for range " << start << " to " << end << std::endl;
}


void barnes_hut_update_step_multi(Scenario &bodies, int num_threads, double time_step) {
    std::cout << "Constructing Barnes-Hut tree...\n";
    QuadNode *root = QuadNode::constructBarnesHutTree(&bodies);
    if (root == nullptr) {
        std::cerr << "Error: root is null" << std::endl;
        return;
    }
    std::cout << "Tree constructed.\n";

    std::vector<std::thread> threads;
    int num_bodies = bodies.r.size();
    int num_bodies_per_thread = num_bodies / num_threads;

    for (int i = 0; i < num_threads - 1; ++i) {
        std::cout << "Starting thread " << i << "\n";
        threads.emplace_back(barnes_hut_update_step_aux, i * num_bodies_per_thread, (i + 1) * num_bodies_per_thread, std::ref(bodies), root, time_step);
    }
    std::cout << "Main thread handling remaining bodies.\n";
    barnes_hut_update_step_aux((num_threads - 1) * num_bodies_per_thread, num_bodies, bodies, root, time_step);

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "Updating positions.\n";
    for (size_t i = 0; i < bodies.r.size(); ++i) {
        if (i >= bodies.v.size()) {
            std::cerr << "Error: Out of bounds access during position update\n";
            return;
        }
        bodies.r[i] += bodies.v[i] * time_step;
    }

    delete root;
    std::cout << "Update complete.\n";
}



void barnes_hut(Scenario &bodies, double time_step, double total_time, 
                std::vector<std::vector<Vector2D>> &all_positions, 
                std::vector<std::vector<Vector2D>> &all_velocities, 
                std::vector<std::vector<Vector2D>> &all_forces, int num_threads) {

    std::cout << "Starting barnes_hut function...\n";

    for (double t = 0; t < total_time; t += time_step) {
        std::cout << "Time: " << t << "\n";
        
        std::cout << "Calling barnes_hut_update_step_multi...\n";
        barnes_hut_update_step_multi(bodies, num_threads, time_step);
        
        std::cout << "barnes_hut_update_step_multi completed.\n";

        // Capture the current state of the system
        if (bodies.r.size() != bodies.v.size() || bodies.r.size() != bodies.f.size()) {
            std::cerr << "Error: Mismatch in sizes of positions, velocities, and forces in barnes_hut\n";
            return;
        }

        all_positions.push_back(bodies.r);
        all_velocities.push_back(bodies.v);
        all_forces.push_back(bodies.f);

        std::cout << "State captured for time: " << t << "\n";
    }

    std::cout << "barnes_hut function complete.\n";
}
