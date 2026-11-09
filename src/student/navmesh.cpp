#include <iostream>
#include <stdexcept>

#include "cpe/navigation.hpp"

namespace cpe
{
    bool NavMesh::Build() {
        built_ = false;
        polygons_.clear();
        portals_.clear();
        graph_.clear();

        constexpr double eps = 1e-6;
        auto same = [&](Vec2 u, Vec2 v) { return Distance(u, v) < eps; };

        
        for (const auto& src : source_) {
            if (src.size() < 3) return false;
            Polygon poly;
            poly.vertices = src;
            Vec2 sum{0, 0};
            for (const Vec2& v : src) {
                if (!Finite(v)) return false;
                sum = sum + v;
            }
            poly.center = sum * (1.0 / static_cast<double>(src.size()));
            polygons_.push_back(poly);
        }

        
        const int n = static_cast<int>(polygons_.size());
        for (int i = 0; i < n; ++i) {
            const auto& vi = polygons_[i].vertices;
            for (int j = i + 1; j < n; ++j) {
                const auto& vj = polygons_[j].vertices;
                bool found = false;

                for (std::size_t ei = 0; ei < vi.size() && !found; ++ei) {
                    const Vec2 a1 = vi[ei];
                    const Vec2 a2 = vi[(ei + 1) % vi.size()];

                    for (std::size_t ej = 0; ej < vj.size() && !found; ++ej) {
                        const Vec2 b1 = vj[ej];
                        const Vec2 b2 = vj[(ej + 1) % vj.size()];

                        if ((same(a1, b2) && same(a2, b1)) ||
                            (same(a1, b1) && same(a2, b2))) {
                            Portal portal;
                            portal.a = i;
                            portal.b = j;
                            portal.p = a1;
                            portal.q = a2;
                            portals_.push_back(portal);
                            found = true;   
                            }
                    }
                }
            }
        }

  
        graph_.assign(polygons_.size(), {});
        for (const Portal& portal : portals_) {
            if (!ValidNode(graph_, portal.a) ||
                !ValidNode(graph_, portal.b) ||
                portal.a == portal.b) {
                throw std::invalid_argument("Invalid Portal indices");
                }

            if (!Finite(portal.p) || !Finite(portal.q) ||
                Distance(portal.p, portal.q) < 1e-8) {
                throw std::invalid_argument("Invalid Portal (non finite or degenerate)");
                }

            if (!Contains(portal.a, portal.p) || !Contains(portal.a, portal.q) ||
                !Contains(portal.b, portal.p) || !Contains(portal.b, portal.q)) {
                throw std::invalid_argument("Portal endpoints outside its polygons");
                }

            const double cost = Distance(polygons_[portal.a].center,
                                         polygons_[portal.b].center);
            AddUndirected(graph_, portal.a, portal.b, cost);
        }

        built_ = true;
        return true;
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
        if (!Finite(point)) return -1;

        const int count = static_cast<int>(Polygons().size());
        for (int i = 0; i < count; ++i) {
            if (Contains(i, point)) return i;
        }
        return -1;
    }
    
    
    bool NavMesh::Contains(int polygon, Vec2 point) const {
        if (polygon < 0 || polygon >= static_cast<int>(Polygons().size())) return false;
        if (!Finite(point)) return false;

        const auto& vertices = Polygons()[polygon].vertices;
        if (vertices.size() < 3) return false;

        constexpr double epsilon = 1e-8;
        bool positive = false, negative = false;

        for (std::size_t i = 0; i < vertices.size(); ++i) {
            const Vec2 a = vertices[i];
            const Vec2 b = vertices[(i + 1) % vertices.size()];
            const double side = Cross(b - a, point - a);
            if (side >= epsilon)  positive = true;
            if (side <= -epsilon) negative = true;
            if (positive && negative) return false;
        }
        return true;
    }
    
    Path NavMesh::FindPath(Vec2 from, Vec2 to, Algorithm algorithm) const
    {
        // TODO J3 : relier localisation, recherche sur graphe et trajet géométrique.
 
        Path result;

        auto noPath = [&]() {
            Path p;
            p.search.status = Status::NoPath;
            return p;
        };
        
        if (!Finite(from) || !Finite(to)) {
            std::cerr << "[FindPath] sortie 1 : point non fini from=("
          << from.x << "," << from.z << ") to=("
          << to.x << "," << to.z << ")\n";
            return result ;
        }

        const int startRegion = Locate(from);
        const int goalRegion = Locate(to);
        const int regionCount = static_cast<int>(Polygons().size());
        
        if (startRegion < 0 || goalRegion < 0 ||
            startRegion >= regionCount || goalRegion >= regionCount
            ) {
            return noPath() ; 
            }
        
        if (startRegion == goalRegion)
        {
            result.search.status = Status::Found;
            result.points.push_back(from);
            if (!Near(from, to)){
                result.points.push_back(to);
            }
            result.length = Distance(from, to);
            return result;
        }
        
        
        switch (algorithm) {
        case Algorithm::AStar: {
            std::vector<double> h;
            h.reserve(regionCount);
            for (int i = 0; i < regionCount; ++i) {
                h.push_back(Distance(Polygons()[i].center,
                                     Polygons()[goalRegion].center));
            }
            result.search = AStar(graph_, startRegion, goalRegion, h);
            break;
        }
        case Algorithm::Dijkstra:
            result.search = Dijkstra(graph_, startRegion, goalRegion);
            break;
        }

        if (result.search.status != Status::Found) {
            return noPath();
        }

        auto append = [&](Vec2 point) {
            if (result.points.empty() || !Near(result.points.back(), point)) {
                result.points.push_back(point);
            }
        };

        append(from);
        append(Polygons()[startRegion].center);

        for (std::size_t i = 1; i < result.search.nodes.size(); ++i) {
            const int previous = result.search.nodes[i - 1];
            const int next     = result.search.nodes[i];

            const Portal* portal = Bet(previous, next);
            if (portal == nullptr) {
                return noPath();   // Correction 2 : statut cohérent
            }

            append(portal->Middle());
            append(Polygons()[next].center);
        }

        append(to);

        result.length = 0.0;
        for (std::size_t i = 1; i < result.points.size(); ++i) {
            result.length += Distance(result.points[i - 1], result.points[i]);
        }
        return result;
    }
    
    
    const Portal* NavMesh::Bet(int a, int b) const
    {
        for (const Portal& portal : Portals()) {
            const bool forward =
                portal.a == a && portal.b == b;

            const bool backward =
                portal.a == b && portal.b == a;

            if (forward || backward) {
                return &portal;
            }
        }
        return nullptr;
    }
}
