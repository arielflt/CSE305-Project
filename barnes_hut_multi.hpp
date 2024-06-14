#ifndef BARNES_HUT_MULTI_HPP
#define BARNES_HUT_MULTI_HPP

#include "nbody_simulation_bhmulti.hpp"
#include <cmath>
#include <stack>
#include <vector>

const double theta = 0.5; // Threshold for the approximation
const double G = 6.67430e-11; // Gravitational constant

class QuadNode {
    enum quad { nw, ne, sw, se };  // indeed this is not used
    bool is_empty = true;
    const Vector2D center;  // center and dimension should be const, right?
    const Vector2D dimension;
    Scenario *const scenario;

public:
    QuadNode *children[4]{nullptr, nullptr, nullptr, nullptr};
    double m = 0;
    Vector2D center_of_mass;
    std::vector<int> body_id;

    // This is the main entry point of the Barnes-Hut tree. This constructs a
    // Barnes-Hut tree from `bodies`.
    // NOTE:: Remember to delete the result.
    static QuadNode *constructBarnesHutTree(Scenario *bodies) {
        QuadNode *root =
            new QuadNode(bodies, Vector2D{500.0, 500.0},
                         Vector2D{1000.0, 1000.0});

        for (size_t i = 0; i < bodies->r.size(); i++) {
            root->addBody(i);
        }

        return root;
    }

    QuadNode(Scenario *const bodies, const Vector2D &center, const Vector2D &dimension)
        : center(center),
          dimension(dimension),
          scenario(bodies),
          center_of_mass(center) {}

    ~QuadNode() {
        for (int i = 0; i < 4; i++) delete children[i];
    }

    bool isFarEnough(const Vector2D &point) const {
        Vector2D dr = center_of_mass - point;
        double dist_sq = std::max(dr.norm2(), 1e-6);
        return dimension.x * dimension.x / dist_sq < theta * theta;
    }

    void addBody(int index);

private:
    quad getQuad(const Vector2D &r) const {
        return r.x < center.x ? (r.y < center.y ? sw : nw)
                              : (r.y < center.y ? se : ne);
    }

    void updateCenterOfMass(size_t id) {
        double new_m = m + scenario->m[id];
        center_of_mass =
            (center_of_mass * m + scenario->r[id] * scenario->m[id]) / new_m;
        m = new_m;
    }

    Vector2D getQuadCenter(quad q) const {
        switch (q) {
            case nw:
                return center + Vector2D{-dimension.x / 4, dimension.y / 4};
            case ne:
                return center + Vector2D{dimension.x / 4, dimension.y / 4};
            case sw:
                return center + Vector2D{-dimension.x / 4, -dimension.y / 4};
            case se:
                return center + Vector2D{dimension.x / 4, -dimension.y / 4};
            default:
                return Vector2D{0, 0};
        };
    }

    bool isInside(const Vector2D &point) const {
        return point.x <= center.x + dimension.x / 2 &&
               point.x >= center.x - dimension.x / 2 &&
               point.y <= center.y + dimension.y / 2 &&
               point.y >= center.y - dimension.y / 2;
    }
};

// Function to update the simulation step with multiple threads
void barnes_hut_update_step_multi(Scenario &bodies, int num_threads);

// Function to perform the update for a subset of bodies
void barnes_hut_update_step_aux(int start, int end, Scenario &bodies, QuadNode *root);

void barnes_hut(Scenario &bodies, double time_step, double total_time, std::vector<std::vector<Vector2D>> &all_positions, std::vector<std::vector<Vector2D>> &all_velocities, std::vector<std::vector<Vector2D>> &all_forces, int num_threads);

#endif // BARNES_HUT_MULTI_HPP
