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
        case State::Idle :
            if ( level.navmesh.Contains(player.AtkRange, enemies[i].position) )
            {
                player.state = State::Windup;
            }
            break;
        
        case State::Windup : 
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
                actionPressed = true;
            }
            break;

        case State::Recover:
            player.GTimer -= dt;
            if (player.GTimer <= 0.0) {
                player.state = State::Idle;
                actionPressed = false;
            }
            break;

        case State::Dead:
            break;
            
        }
    }


void Wave::Waves(int waves, std::vector<Enemy>enemies, bool victory )
{
    
    int wave = waves;
    
    victory = false;
    
    int enemiesCount = enemies.size();
    
    if (enemiesCount <= 0)
    {
        wave++;
        enemiesCount *=  multiplier;
        enemies.resize(enemiesCount);
        std::cout << "waves " << wave << std::endl;
    }
    
    if ( wave > 5)
    {
        victory = true;
    }
    
}
}
    

