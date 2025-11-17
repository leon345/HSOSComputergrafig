#ifndef CapsuleCollision_hpp
#define CapsuleCollision_hpp
 
#include "Aabb.h"
#include "vector.h"

class CapsuleCollision
{
public:
    Vector tip;
    Vector base;
    float radius;
    const float EPS = 1e-6f;

    CapsuleCollision(const Vector &tip, const Vector &base, const float radius);

    // Checks collision on a sphere
    bool intersectsSphere(const Vector &center, float radius) const;

    // Checks collision on a AABB-box
    bool segmentIntersectsAABB(const Vector& p0, const Vector& p1, const Vector& bMin, const Vector& bMax) const;
    
    Vector closestPointOnSegment(const Vector &point) const;

    float squaredDistancePointAABB(const Vector &point, const AABB &box) const;

    bool intersectsAABB(const AABB &box) const;

    Vector closestPointOnTriangle(const Vector& p, const Vector& a, const Vector& b, const Vector& c) const;

    void closestPointsSegmentSegment(const Vector& p1, const Vector& q1, const Vector& p2, const Vector& q2, Vector& c1,
                                     Vector& c2) const;

    bool intersectsTriangle(const Vector& a, const Vector& b, const Vector& c, Vector& collisionNormal,
                            float& penetrationDepth) const;
};

#endif // !CapsuleCollision_hpp


