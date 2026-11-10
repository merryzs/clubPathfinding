#include <iostream>

#include "cpe/simulation.hpp"
namespace cpe {
void Simulation::UpdateGameplay(bool actionPressed, double dt)
{
    // TODO PROJET : attaques, dégâts, vie, mort, vagues, victoire et défaite.
    // Espace alimente actionPressed ; aucun effet de jeu n'est fourni.
    
    if (player.hp <= 0 )
    {
        player.state = State::Dead;
    }
    
    for ( auto i = 0; i < enemies.size(); i++ )
    {
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
            if (player.GTimer <= 0 )
            {
                if ( level.navmesh.Contains(player.AtkRange, enemies[i].position))
                {
                    enemies[i].hp = std::max(0, enemies[i].hp - player.dmg);
                    std::cout << "attaque enemy" << std::endl;
                }
                player.state = State::Recover;
                player.GTimer = player.PostCastCd;
            }
            break;
        
        case State::Recover : 
            player.GTimer -= dt;
            if (player.GTimer <= 0 )
            {
                player.state = State::Idle;
            }
            break;
        
        case State::Dead :
            break;
            
        }
        
        if ( wave.victory)
        {
            std::cout << "victory" << std::endl;
        }
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
    

