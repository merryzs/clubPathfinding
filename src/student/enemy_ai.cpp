#include <algorithm>
#include <iostream>

#include "cpe/simulation.hpp"

namespace cpe {

void Simulation::UpdateEnemy(Enemy& enemy, double dt) {
    // Ennemi déjà mort : il sera retiré par Update()
    if (enemy.state == State::Dead) return;

    // Passage à l'état mort (une seule fois)
    if (enemy.hp <= 0) {
        enemy.state = State::Dead;
        enemy.navigator.Invalidate();
        std::cout << "Enemy Dead " << (&enemy - enemies.data()) << std::endl;
        return;
    }

    enemy.Delay(dt);

    // Joueur mort : l'ennemi se calme
    if (player.hp <= 0) {
        enemy.state = State::Idle;
        enemy.navigator.Invalidate();
        return;
    }

    const double dist = Distance(enemy.position, player.position);

    switch (enemy.state)
    {
    case State::Waiting:
        // À remplacer par la condition de fin du délai d'apparition si besoin
        enemy.state = State::Idle;
        break;

    case State::Idle:
        if (dist <= enemy.DetctRange) {
            std::cout << "Idle -> Chase (dist=" << dist << ")\n";
            enemy.state = State::Chase;
        }
        break;

    case State::Chase:
        if (dist <= enemy.AtkRange) {               // 1) assez près : attaquer
            enemy.state  = State::Windup;
            enemy.GTimer = enemy.CastTime;
            enemy.navigator.Invalidate();
        }
        else if (dist > enemy.LoseRange) {          // 2) joueur perdu
            enemy.state = State::Idle;
            enemy.navigator.Invalidate();
        }
        else {                                      // 3) poursuite
            enemy.navigator.Plan(level, enemy.position, player.position, algorithm);
            enemy.navigator.Follow(level, player.position, enemy.speed, dt);
        }
        break;

    case State::Windup:
        enemy.GTimer -= dt;
        if (enemy.GTimer <= 0.0) {
            if (dist <= enemy.AtkRange) {
                player.hp = std::max(0, player.hp - enemy.dmg);
                std::cout << "Enemy Attack (HP Player " << player.hp << ")\n";
            }
            enemy.state  = State::Recover;
            enemy.GTimer = enemy.PostCastCd;
        }
        break;

    case State::Recover:
        enemy.GTimer -= dt;
        if (enemy.GTimer <= 0.0) {
            enemy.state = State::Chase;
        }
        break;

    case State::Dead:
        break;
    }
}

} // namespace cpe