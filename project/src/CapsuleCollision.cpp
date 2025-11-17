#include "CapsuleCollision.h"
#include <cmath>
#include "Mathf.h"

CapsuleCollision::CapsuleCollision(const Vector& tip, const Vector& base, const float radius) : tip(tip), base(base), radius(radius) {}

//  Quelle: Real-Time Collision Detection von Chester Ericson Kapitel 5.1.4
// http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
Vector CapsuleCollision::closestPointOnSegment(const Vector& point) const
{
    Vector segment = base - tip;
    float denom = segment.dot(segment);
    if(denom <= 10e-4f)
    {
        return tip;
    }
    float t = (point - tip).dot(segment) / denom;
    t = Mathf::clamp(t, 0.0f, 1.0f);

    return tip + segment * t;
}

// Quelle: Ericson Kapitel 5.1.9
bool CapsuleCollision::intersectsSphere(const Vector& center, float otherRadius) const
{
    Vector closest = closestPointOnSegment(center);
    float distanceSquared = (closest - center).lengthSquared();
    float totalRadius = (radius + otherRadius);
    return distanceSquared < totalRadius * totalRadius;
}

// Quelle: Ericson 5.1.3
float CapsuleCollision::squaredDistancePointAABB(const Vector& point, const AABB& box) const
{
    float sqDist = 0.0f;
    // X
    if(point.X < box.Min.X)
    {
        float dist = point.X - box.Min.X;
        sqDist += dist * dist;
    } else if (point.X > box.Max.X)
    {
        float dist = point.X - box.Max.X;
        sqDist += dist * dist;
    }

    // Y
    if(point.Y < box.Min.Y)
    {
        float dist = point.Y - box.Min.Y;
        sqDist += dist * dist;
    } else if (point.Y > box.Max.Y)
    {
        float dist = point.Y - box.Max.Y;
        sqDist += dist * dist;
    }

    // Z
    if(point.Z < box.Min.Z)
    {
        float dist = point.Z - box.Min.Z;
        sqDist += dist * dist;
    } else if (point.Z > box.Max.Z)
    {
        float dist = point.Z - box.Max.Z;
        sqDist += dist * dist;
    }

    return sqDist;
}

// Quelle: Ericson Kapitel 5.3.3
bool CapsuleCollision::segmentIntersectsAABB(const Vector& p0, const Vector& p1, const Vector& bMin, const Vector& bMax) const
{
    Vector d = p1 - p0;
    float tMin = 0.0f, tMax = 1.0f;

    // Für alle 3 Slabs
    for(int i = 0; i < 3; ++i)
    {
        float origin = (i == 0 ? p0.X : (i == 1 ? p0.Y : p0.Z));
        float dir    = (i == 0 ? d.X : (i == 1 ? d.Y : d.Z));
        float minB   = (i == 0 ? bMin.X : (i == 1 ? bMin.Y : bMin.Z));
        float maxB   = (i == 0 ? bMax.X : (i == 1 ? bMax.Y : bMax.Z));

        if(std::fabs(dir) < std::numeric_limits<float>::epsilon())
        {
            // ist parallel zu den slabs slabs?
            if(origin < minB || origin > maxB) return false;
        } else
        {
            // Intersection berechnet
            float ood = 1.0f / dir;
            float t1 = (minB - origin) * ood;
            float t2 = (maxB - origin) * ood;
            if(t1 > t2) std::swap(t1, t2);
            tMin = std::fmin(tMin, t1);
            tMax = std::fmax(tMax, t1);
            if(tMin > tMax) return false;
        }
    }
    // Schnitt im Parameterbereich [0,1] des Segments?
    return !(tMax < 0.0f || tMin > 1.0f);
}

bool CapsuleCollision::intersectsAABB(const AABB& box) const
{
    // Erweitere die Box um Radius und teste Segment gegen erweiterte Box (Minkoswki-Sum)
    Vector minE = box.Min - Vector(radius, radius, radius);
    Vector maxE = box.Max + Vector(radius, radius, radius);
    return segmentIntersectsAABB(tip, base, minE, maxE);
}

// Quelle: Ericson Kapitel 5.1.5
Vector CapsuleCollision::closestPointOnTriangle(const Vector& p, const Vector& a, const Vector& b, const Vector& c) const {
    // Check vertex region outside A
    Vector ab = b - a;
    Vector ac = c - a;
    Vector ap = p - a;
    float d1 = ab.dot(ap);
    float d2 = ac.dot(ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    // Check vertex region outside B
    Vector bp = p - b;
    float d3 = ab.dot(bp);
    float d4 = ac.dot(bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    // Check edge region AB
    float vc = d1*d4 - d3*d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + ab * v;
    }

    // Check vertex region outside C
    Vector cp = p - c;
    float d5 = ab.dot(cp);
    float d6 = ac.dot(cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    // Check edge region AC
    float vb = d5*d2 - d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + ac * w;
    }

    // Check edge region BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + (c - b) * w;
    }

    // Inside face region. Project P onto plane
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

// Quelle: Ericson 5.1.9
void CapsuleCollision::closestPointsSegmentSegment(const Vector& p1, const Vector& q1, const Vector& p2, const Vector& q2, Vector& c1, Vector& c2) const {
    Vector d1 = q1 - p1; // direction vector of segment S1
    Vector d2 = q2 - p2; // direction vector of segment S2
    Vector r = p1 - p2;
    float a = d1.dot(d1); // squared length of segment S1
    float e = d2.dot(d2); // squared length of segment S2
    float f = d2.dot(r);

    float s, t;

    if (a <= EPS && e <= EPS) {
        // both segments degenerate to points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return;
    }
    if (a <= EPS) {
        // first segment degenerate (point)
        s = 0.0f;
        t = Mathf::clamp(f / e, 0.0f, 1.0f);
    } else {
        float c = d1.dot(r);
        if (e <= EPS) {
            // second segment degenerate
            t = 0.0f;
            s = Mathf::clamp(-c / a, 0.0f, 1.0f);
        } else {
            float b = d1.dot(d2);
            float denom = a*e - b*b;
            if (denom != 0.0f) s = Mathf::clamp((b*f - c*e) / denom, 0.0f, 1.0f);
            else s = 0.0f;
            t = (b*s + f) / e;
            if (t < 0.0f) {
                t = 0.0f;
                s = Mathf::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = Mathf::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
}

// ---- Main triangle intersection test ----
bool CapsuleCollision::intersectsTriangle(const Vector& a, const Vector& b, const Vector& c, Vector& collisionNormal, float& penetrationDepth) const {
    // Compute triangle normal
    Vector ab = b - a;
    Vector ac = c - a;
    Vector N = ab.cross(ac);
    float nLen2 = N.lengthSquared();
    if (nLen2 < EPS) return false; // degenerate triangle
    N = N * (1.0f / sqrtf(nLen2)); // normalize

    Vector seg = base - tip;
    float segLen2 = seg.dot(seg);
    if (segLen2 < EPS) return false; // degenerate capsule axis

    // 1) Check intersection of segment with triangle plane
    float denom = N.dot(seg);
    // If not parallel:
    if (std::abs(denom) > 1e-6f) {
        float t = N.dot(a - tip) / denom; // parameter along segment
        if (t >= 0.0f && t <= 1.0f) {
            Vector p = tip + seg * t; // point on segment that lies in plane
            // check if p is inside triangle (edge tests)
            Vector c0 = (p - a).cross(b - a);
            Vector c1 = (p - b).cross(c - b);
            Vector c2 = (p - c).cross(a - c);
            bool inside = (c0.dot(N) >= 0.0f && c1.dot(N) >= 0.0f && c2.dot(N) >= 0.0f) ||
                          (c0.dot(N) <= 0.0f && c1.dot(N) <= 0.0f && c2.dot(N) <= 0.0f);
            if (inside) {
                // distance from segment axis to triangle plane is 0 at this t -> collision if radius > 0
                float dist = 0.0f;
                if (radius > dist) {
                    // normal should point from triangle to capsule segment:
                    float sgn = denom > 0.0f ? -1.0f : 1.0f; // if seg points in same dir as N, normal is -N
                    collisionNormal = N * sgn;
                    penetrationDepth = radius - dist;
                    return true;
                }
            }
        }
    }

    // 2) Else compute minimal distance between segment and triangle:
    // candidate A: closest point from segment endpoints to triangle
    float bestSq = std::numeric_limits<float>::max();
    Vector bestSegPt, bestTriPt;

    // endpoints
    {
        Vector cp = closestPointOnTriangle(tip, a, b, c);
        float sq = (tip - cp).lengthSquared();
        if (sq < bestSq)
        {
            bestSq = sq;
            bestSegPt = tip;
            bestTriPt = cp;
        }
    }
    {
        Vector cp = closestPointOnTriangle(base, a, b, c);
        float sq = (base - cp).lengthSquared();
        if (sq < bestSq)
        {
            bestSq = sq;
            bestSegPt = base;
            bestTriPt = cp;
        }
    }

    // candidate B: closest between segment and each triangle edge (segment-segment)
    Vector segStart = tip, segEnd = base;
    Vector e1s = a, e1e = b;
    Vector c1, c2;
    closestPointsSegmentSegment(segStart, segEnd, e1s, e1e, c1, c2);
    {
        float sq = (c1 - c2).lengthSquared();
        if (sq < bestSq)
        {
            bestSq = sq;
            bestSegPt = c1;
            bestTriPt = c2;
        }
    }
    closestPointsSegmentSegment(segStart, segEnd, b, c, c1, c2);
    {
        float sq = (c1 - c2).lengthSquared();
        if (sq < bestSq) { bestSq = sq;
            bestSegPt = c1;
            bestTriPt = c2; }
    }
    closestPointsSegmentSegment(segStart, segEnd, c, a, c1, c2);
    {
        float sq = (c1 - c2).lengthSquared();
        if (sq < bestSq)
        {
            bestSq = sq;
            bestSegPt = c1;
            bestTriPt = c2;
        }
    }

    float dist = sqrtf(bestSq);
    if (dist < radius) {
        // penetration vector from triangle to segment
        Vector diff = bestSegPt - bestTriPt;
        if (diff.lengthSquared() < 1e-6f) {
            // fallback normal = triangle normal
            collisionNormal = N;
        } else {
            collisionNormal = diff * (1.0f / dist); // normalized
        }
        penetrationDepth = radius - dist;
        return true;
    }

    return false;
}




