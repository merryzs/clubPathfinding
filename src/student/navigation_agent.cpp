#include "cpe/simulation.hpp"


namespace cpe
{
    void Navigator::Plan(const Level& level, Vec2 position,
                         Vec2 target, Algorithm algorithm) {
        
        if (requestCooldown > 0.0) return;

        const double r = level.agentRadius;
        const bool changed = !hasGoal
                          || Distance(target, lastGoal) > 0.75
                          || algorithm != lastAlgorithm;
        
        const bool failed = path.search.status != Status::Found;
        const bool retry = failed && failures < kMaxFailures;
        
        
        const bool finished = !failed && next >= path.points.size();
        const bool stoppedFar = finished
            && !path.points.empty()
            && Distance(position, target) > r
            && Distance(path.points.back(), target) <= r;

        if (!changed && !retry && !stoppedFar && !needsReplan) return;

        if (changed) failures = 0;

        path = level.navmesh.FindPath(position, target, algorithm);
        next = path.points.size() > 1 ? 1 : path.points.size();
        lastGoal = target;
        lastAlgorithm = algorithm;
        hasGoal = true;
        needsReplan = false;
        ++requests;

        if (path.search.status == Status::Found) {
            failures = 0;
            requestCooldown = 0.3;
        } else {
            ++failures;
            requestCooldown = std::min(0.3 * std::pow(2.0, failures - 1), 3.0);
        }
    }
    

    Vec2 Navigator::Follow(const Level& level, Vec2 position, double speed, double dt)
    {
        // TODO J4 : consommer les waypoints et appliquer un déplacement valide.
        
        if (path.search.status != Status::Found || dt <= 0 || speed <= 0)
            return position;
        
        const double r = level.agentRadius;
        double budget = speed * dt;

        while (next < path.points.size() && budget > 0) {
            Vec2 delta = path.points[next] - position;
            double distance = Length(delta);

            if (distance < 1e-8) { ++next; continue; }

            const bool reached = distance <= budget;
            Vec2 target = reached
                ? path.points[next]
                : position + delta * (budget / distance);

            if (level.CanMove(position, target, r)) {
                position = target;
            } else {
                // Pas direct bloqué : on tente de glisser le long de l'obstacle
                Vec2 slid = level.MoveSliding(position, target - position, r);
                if (Length(slid - position) < 1e-8)
                    return position;   
                return slid;          
            }
            
            if (reached) {
                budget -= distance;
                ++next;
            } else {
                budget = 0;
            }
        }
        return position;
    }
}

