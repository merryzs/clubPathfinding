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
        if ()
        {
            
        }
        break;

    case State::Chase : 
        // boucle if a faire
        if ()
        {
            enemy.navigator.path.search 
        }
        else
        {
            enemy.navigator.Plan( level, position, target, algorithm);
            enemy.navigator.Follow( level, position, speed , dt);
        }
        break;

    case State::Windup :
        enemy.GTimer -= dt;
        
        if (enemy.GTimer <= 0 )
        {
            //distance a faire quand possible
            if ()
            {
                
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


