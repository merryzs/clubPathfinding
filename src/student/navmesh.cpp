#include <queue>
#include <stdexcept>

#include <cpe/navigation.hpp>
namespace cpe {
bool NavMesh::Build() {
    // TODO J3 : valider et structurer source_, calculer les adjacences/portails.
    // Remplir polygons_, portals_, graph_, puis définir built_=true.
    
    graph_.assign(polygons_.size(), {});
    for (const Portal& portal : portals_)
    {
        if (!ValidNode(graph_, portal.a) ||
            !ValidNode(graph_, portal.b) ||
            portal.a == portal.b)
        {
            throw std::invalid_argument("Invalid Portal indicices");
        }
        
        if (!Finite(portal.p) || !Finite(portal.q) || Distance(portal.p, portal.q) < 1e-8)
        {
            throw std::invalid_argument("Invalid Portal or null");
        }
        
        
        if (!Contains(portal.a , portal.p) || !Contains(portal.a , portal.q) || !Contains(portal.b, portal.p) || !Contains(portal.b, portal.q))
        {
            throw std::invalid_argument("Invalid Portal or null");
        }
        
        const Vec2 middle = (Polygons()[portal.a].center + Polygons()[portal.b].center) * 0.5;
        
        const double cost = Distance(Polygons()[portal.a].center, middle) + Distance(middle, Polygons()[portal.b].center);
    
        AddUndirected(graph_, portal.a, portal.b, cost);
    }
    
    
    return false;
}
int NavMesh::Locate(Vec2 point) const {
    // TODO J3 : localiser le point dans un polygone navigable.
    (void)point;
    return -1;
}
bool NavMesh::Contains(int polygon, Vec2 point) const {
    // TODO J3 : test point dans polygone.
    (void)polygon;
    (void)point;
    return false;
}
bool NavMesh::SegmentInside(Vec2 from, Vec2 to) const {
    // TODO J3 : vérifier qu'un segment reste dans la surface navigable.
    (void)from;
    (void)to;
    return false;
}
    
Path NavMesh::FindPath(Vec2 from, Vec2 to, Algorithm algorithm) const {
    // TODO J3 : relier localisation, recherche sur graphe et trajet géométrique.
 
     Path result;

    if (!Finite(from) || !Finite(to)) {
        return result ;
    }

    const int startRegion = Locate(from);
    const int goalRegion = Locate(to);

    if (startRegion < 0 || goalRegion < 0) {
        result.search.status = Status::OutsideNavMesh;
        return result;
    }
    
    // calculer tout les sommets et les add dans h
    std::vector<double> h;
    
    for (int i = 0; i < static_cast<int>(Polygons().size()); i++)
    {
        h.push_back(Distance(Polygons()[i].center, Polygons()[goalRegion].center));
    }
    
    switch (algorithm)
    {
    case Algorithm::AStar :
        result.search =  AStar(graph_, startRegion, goalRegion, h); 
        break;
        
    case Algorithm::Dijkstra :
        result.search =  Dijkstra(graph_, startRegion, goalRegion);
        break;
    }

    if (result.search.status != Status::Found) {
        result.search.status = Status::NoPath;
        return result;
    }

    auto append = [&](Vec2 point) {
        if (result.points.empty() ||
            !Near(result.points.back(), point)) {
            result.points.push_back(point);
            }
    };

    append(from);

    if (startRegion != goalRegion)
    {
        append(Polygons()[startRegion].center);

        for (std::size_t i = 1;
            i < result.search.nodes.size();
            ++i)
        {
            const int previous = result.search.nodes[i - 1];
            const int next = result.search.nodes[i];

            const Portal* portal =
                Bet(previous, next);

            if (portal == nullptr) {
                return Path{};
            }
            
            append(portal->Middle());
            append(Polygons()[next].center);
        }
    }

    append(to);

    result.length = 0.0;

    for (std::size_t i = 1; i < result.points.size(); ++i) {
        result.length += Distance(
            result.points[i - 1],
            result.points[i]
        );
    }
    return result;
    
}
}
