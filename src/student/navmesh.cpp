#include <stdexcept>

#include "cpe/navigation.hpp"
namespace cpe {
bool NavMesh::Build() {
    // TODO J3 : valider et structurer source_, calculer les adjacences/portails.
    // Remplir polygons_, portals_, graph_, puis définir built_=true.
    
    graph_.assign(polygons_.size(), {});
    for (const Portal& portal : portals_)
    {
        if (!ValidNod(graph_, portal.a) ||
            !ValidatePortal(graph_, portal.b) ||
            portal.a == portal.b)
        {
            throw std::invalid_argument("Invalid Portal indicices");
        }
        
        if (!Finite(portal.left) || !Finite(portal.right) || Distance(portal.left, portal.right) < 1e-8)
        {
            throw std::invalid_argument("Invalid Portal or null");
        }
        
        const Polygon& regionA = polygons_[portal.a];
        const Polygon& regionB = polygons_[portal.b];
        
        if (!Contains(regionA, portal.left) || !Contains(regionA, portal.right) || !Contains(regionB, portal.left) || !Contains(regionB, portal.right))
        {
            throw std::invalid_argument("Invalid Portal or null");
        }
        
        const Point middle = portal.Middle();
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
    (void)from;
    (void)to;
    (void)algorithm;
    return {};
}
}
