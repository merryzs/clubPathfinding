#include "cpe/navigation.hpp"
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

bool NavMesh::SegmentInside(Vec2 from, Vec2 to) const {
    // TODO J3 : vérifier qu'un segment reste dans la surface navigable.

    if (!Finite(from) || !Finite(to)) return false;
    
    std::vector<std::pair<double,double>> interavals;
    
    const auto directions = to-from;
    
    for (const Polygon& polygon : Polygons())
    {
        const auto vertices = polygon.vertices;
        double orientation = Cross(vertices[1] - vertices[0], vertices[2]-vertices[1]) > 0 ? 1.0 : -1.0;
        double low = 0.0, high = 1.0;
        bool valid = true;
        for ( std::size_t i = 0;i < vertices.size(); ++i)
        {
            Vec2 edge = vertices[i+1%vertices.size()]-vertices[i];
            double base = orientation*Cross(edge, from-vertices[i]);
            double slope = orientation*Cross(edge, directions);
            
            if (std::abs(slope) < 1e-12)
            {
                if (base < -1e9){valid = false; break; }
            } else
            {
                double bound = (-1e9-base)/slope;
                if (slope > 0) low = std::max(low, bound);
                else high = std::min(high, bound);
            }
        }
        if (valid && low <=high) interavals.push_back({low,high});
    }
    std::sort(interavals.begin(), interavals.end());
    double covered = 0.0;
    for (auto [low,high] : interavals)
    {
        if ( low > covered +1e-8) return false;
        covered = std::max (covered, high);
        if (covered >= 1.0-1e-8)return true;
    }
    return false;
}

int NavMesh::Locate(Vec2 point) const {
    // TODO J3 : localiser le point dans un polygone navigable.
    (void)point;
    if (!Finite(point)) return -1;
    for (int i = 0; i < static_cast<int>(Polygons().size()); ++i)
    {
        if (Contains(i, point)) return i;
    }
    return -1;
}

bool NavMesh::Contains(int polygon, Vec2 point) const
{
    // TODO J3 : test point dans polygone.
    if (Finite(point) || Polygons()[polygon].vertices.size() < 3) return false;
    constexpr double epsilon = 1e-8;
    bool positive = false, negative = false;
    const auto& vertices = Polygons()[polygon].vertices;
    
    for (std::size_t i = 0;i < vertices.size();++i) {
        const Vec2 a = vertices[i];
        const Vec2 b = vertices[(i + 1) % vertices.size()];
        const double side = Cross(b - a, point - a);
        if (side >= epsilon) positive = true;
        if (side < -epsilon) negative = true;
        if (positive && negative) return false;
    }
    return true;
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
