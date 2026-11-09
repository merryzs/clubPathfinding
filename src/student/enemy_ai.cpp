#include "cpe/simulation.hpp"

namespace cpe {
void Simulation::UpdateEnemy(Enemy& enemy, double dt) {
    // TODO PROJET : perception, transitions, décisions et appel du Navigator.
    // Les ennemis restent immobiles tant que ce module n'est pas implémenté.
    (void)enemy;
    (void)dt;
    
    enemy.Delay(dt);
    
    if (enemy.hp <= 0 )
    {
        enemy.state = State::Dead; 
        enemy.navigator.Invalidate(); 
        return ;
    }
    
    if (enemy.state == State::Dead)
    {
        return;
    }
    
    switch (enemy.state)
    {
    case State::Waiting :
        break;

    case State::Idle : 
        //boucle if a faire
        if ( enemy.navmesh.Contains(enemy.DetctRange, player.position))
        {
         enemy.state = State::Chase;   
        }
        break;

    case State::Chase : 
        
        enemy.navigator.Follow(level, player.position, enemy.speed, dt);
        
        // boucle if a faire
        // Attack
        if (enemy.navmesh.Contains(enemy.AtkRange, player.position))
        {
            enemy.state = State::Windup;
            enemy.GTimer = enemy.CastTime;
        }
        // Lose player 
        if (!enemy.navmesh.Contains(enemy.LoseRange, player.position))
        {
            enemy.state = State::Idle;
        }
        //
        else
        {
            enemy.navigator.Plan( level, enemy.position, player.position, algorithm);
            enemy.navigator.Follow( level, player.position, enemy.speed , dt);
        }
        break;

    case State::Windup :
        enemy.GTimer -= dt;
        
        if (enemy.GTimer <= 0 )
        {
            
            //distance a faire quand possible
            if ( enemy.navmesh.Contains(enemy.AtkRange, player.position))
            {
                player.hp = std::max(0, player.hp - enemy.dmg);
            }
            
            enemy.state = State::Recover;
            enemy.GTimer = enemy.PostCastCd;
        }
        break;

    case State::Recover :   
        enemy.GTimer -= dt;
        
        if (enemy.GTimer <= 0 )
        {
            enemy.state = State::Chase;
        }
        break;
        
    case State::Dead :
        break;
        
    }
    
    
    
    
}
}


