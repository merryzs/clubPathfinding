#include <stdexcept>

#include "cpe/navigation.hpp"
namespace cpe {
SearchResult Dijkstra(const Graph& graph, int start, int goal) {
    // TODO J1 : implémenter et instrumenter Dijkstra.
    (void)graph;
    (void)start;
    (void)goal;
    return {};
}
SearchResult AStar(const Graph& graph, int start, int goal,
                   const std::vector<double>& heuristic) {
    // TODO J2 : implémenter A* et justifier le contrat de l'heuristique.
    (void)graph;
    (void)start;
    (void)goal;
    (void)heuristic;
    return {};
    
    // mmmmmm...
    
    
    for (auto i = 0 ; i < graph.size() ; ++i)
    {
        if (!Finite(graph[i])) throw std::invalid_argument("start or goal aren't finite");
        
        for (auto j = 0 ; j < i; ++j)
        {
            if (Near(start, goal )) throw std::invalid_argument("start and goal are similar");

        }
    }
    if (NavMesh::SegmentInside(start,goal))
    {
        SearchResult.status = Status::Found;
    }
    
    
}
}


