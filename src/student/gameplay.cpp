#include <algorithm>
#include <iostream>

#include "cpe/simulation.hpp"

namespace cpe {

    void Simulation::UpdateGameplay(bool actionPressed, double dt) {
        // Joueur mort : plus rien ne se passe
        if (player.hp <= 0) {
            player.state = State::Dead;
            return;
        }

        switch (player.state)
        {
        case State::Idle:
            // Espace déclenche la préparation de l'attaque
            if (actionPressed) {
                player.state  = State::Windup;
                player.GTimer = player.PostCastCd;       // durée du Windup
            }
            break;

        case State::Windup:
            player.GTimer -= dt;
            if (player.GTimer <= 0.0) {
                // La portée est vérifiée à la fin du Windup, sur tous les ennemis
                for (auto& e : enemies) {
                    if (e.hp <= 0) continue;            // on ignore les morts

                    const double d = Distance(player.position, e.position);
                    if (d <= player.AtkRange) {
                        e.hp = std::max(0, e.hp - player.dmg);
                        std::cout << "attaque enemy (hp restants : " << e.hp << ")\n";
                    }
                }
                player.state  = State::Recover;
                player.GTimer = player.PostCastCd;      // durée du Recover
            }
            break;

        case State::Recover:
            player.GTimer -= dt;
            if (player.GTimer <= 0.0) {
                player.state = State::Idle;
            }
            break;

        case State::Dead:
            break;
        }
    }

} // namespace cpe