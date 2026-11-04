#include "cpe/navigation.hpp"
namespace cpe {
bool NavMesh::Build() {
    // TODO J3 : valider et structurer source_, calculer les adjacences/portails.
    // Remplir polygons_, portals_, graph_, puis définir built_=true.
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
    (void)from;
    (void)to;
    (void)algorithm;
    return {};
}
}
