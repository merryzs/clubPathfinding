#pragma once
#include "level.hpp"
#include <cstddef>

namespace cpe {
    
    enum class State { Waiting, Idle, Chase, Windup, Recover, Dead };
    const char* Name(State value);
    


// Ce composant expose les données au debug ; le suivi est à implémenter.
    struct Navigator {
        Path path;
        std::size_t next = 0;
        std::size_t requests = 0;

        // État de planification
        Vec2 lastGoal;
        bool hasGoal = false;
        double requestCooldown = 0.0;   
        int failures = 0;               
        bool needsReplan = false;   
        
        Algorithm lastAlgorithm = Algorithm::AStar;
        static constexpr int kMaxFailures = 5;

        
        void Invalidate();
        void Plan(const Level& level, Vec2 position, Vec2 target, Algorithm algorithm);
        Vec2 Follow(const Level& level, Vec2 position, double speed, double dt);
    };
    


// Entités de démonstration immobiles ; aucun comportement n'est fourni.
// Ajoutez ici les données nécessaires à votre propre IA et votre combat.
struct Enemy {
    Vec2 position;
    EnemyKind kind = EnemyKind::Light;
    State state = State::Waiting;
    Navigator navigator;
    //some state
    int hp = 40;
    int dmg = 10;
    
    double speed = 2.0;
    
    double DetctRange = 3;
    double AtkRange = 1;
    double LoseRange = 4;
    
    double CastTime = 0.5;
    double PostCastCd = 0.5;
    double GTimer = 0.0;
    double cd = 0.0;
    
    void Delay(double dt)
    {
        cd = std::max(0.0, cd-dt);
    };
};
    
    struct Player
    {
        Vec2 position; Vec2 facing{0,-1};
    
        //other info 
        State state = State::Idle;
    
        int hp = 100;
        int dmg = 20;
        
        double AtkRange = 5;
        
        double CastTime = 1.0;
        double PostCastCd = 1.0;
        double GTimer = 0.0;
        double cd = 0.0;
        
        void Delay(double dt)
        {
            cd = std::max(0.0, cd-dt);
        };
        
    
    };
    
    struct Wave
    {
        int waves = 0;
        
        std::vector<Enemy> enemies;
        double multiplier = 1.618;
        
        void Wave(waves, enemies, multiplier);
        
        void Waves( int waves, std::vector<Enemy> enemies, bool victory);
    };
    
class Simulation {
public:
    explicit Simulation(Level level);
    Level level;
    Player player;
    Wave wave;
    std::vector<Enemy> enemies;
    Algorithm algorithm = Algorithm::AStar;
    double elapsed = 0;

    // Fourni : déplacement, collisions, boucle et appels des points d'extension.
    void Update(Vec2 movement, bool actionPressed, double dt);
    void Reset();
    void SetAlgorithm(Algorithm value);
    std::size_t RequestCount() const;
    

private:
    // À réaliser dans src/student/enemy_ai.cpp et gameplay.cpp.
    void UpdateEnemy(Enemy& enemy, double dt);
    void UpdateGameplay(bool actionPressed, double dt);
    void wave(Enemy& enemy, double dt);
    
    
};
}
