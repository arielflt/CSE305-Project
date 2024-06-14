#ifndef BARNES_HUT_MULTI_HPP
#define BARNES_HUT_MULTI_HPP

#include "nbody_simulation_bhmulti.hpp"
#include <cmath>
#include <stack>
#include <vector>

const double theta = 0.5; // Threshold for the approximation
const double G = 6.67430e-11; // Gravitational constant

class QuadNode {
public:
    enum quad { nw, ne, sw, se };
    bool is_empty = true;
    const Vector2D center;
    const Vector2D dimension;
    Scenario *const scenario;

    QuadNode *children[4]{nullptr, nullptr, nullptr, nullptr};
    double m = 0;
    Vector2D center_of_mass;
    std::vector<int> body_id;

    static QuadNode *constructBarnesHutTree(Scenario *bodies);
    QuadNode(Scenario *const bodies, const Vector2D &center, const Vector2D &dimension);
    ~QuadNode();

    bool isFarEnough(const Vector2D &point) const;
    void addBody(int index);  // Ensure this declaration is here

private:
    quad getQuad(const Vector2D &r) const;
    void updateCenterOfMass(size_t id);
    Vector2D getQuadCenter(quad q) const;
    bool isInside(const Vector2D &point) const;
};

void barnes_hut_update_step_multi(Scenario &bodies, int num_threads, double time_step);
void barnes_hut_update_step_aux(int start, int end, Scenario &bodies, QuadNode *root, double time_step);
void barnes_hut(Scenario &bodies, double time_step, double total_time, std::vector<std::vector<Vector2D>> &all_positions, std::vector<std::vector<Vector2D>> &all_velocities, std::vector<std::vector<Vector2D>> &all_forces, int num_threads);

#endif // BARNES_HUT_MULTI_HPP