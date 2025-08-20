#pragma once
#include "hittable.h"

// Infinite plane with checker texture (alternating colors along U/V)
struct Plane : public Hittable {
    Vec3 point;   // any point on plane
    Vec3 normal;  // normalized
    Material matA; // color A
    Material matB; // color B
    double scale; // checker size (world units)

    Plane(const Vec3&p, const Vec3&n, const Material&a, const Material&b, double s)
        : point(p), normal(normalize(n)), matA(a), matB(b), scale(s) {}

    std::optional<HitRecord> hit(const Ray& r, double tmin, double tmax) const override {
        double denom = dot(normal, r.dir);
        if (std::fabs(denom) < 1e-6) return std::nullopt;
        double t = dot(point - r.origin, normal) / denom;
        if (t < tmin || t > tmax) return std::nullopt;

        Vec3 pHit = r.at(t);

        // Build a local axis for UVs
        Vec3 uAxis = normalize(std::fabs(normal.x) > 0.9 ? cross(Vec3{0,1,0}, normal) : cross(Vec3{1,0,0}, normal));
        Vec3 vAxis = cross(normal, uAxis);

        double u = dot(pHit, uAxis) / scale;
        double v = dot(pHit, vAxis) / scale;

        bool useA = ( (static_cast<int>(std::floor(u)) + static_cast<int>(std::floor(v))) & 1 ) == 0;
        Material m = useA ? matA : matB;

        HitRecord rec;
        rec.t = t;
        rec.p = pHit;
        rec.normal = denom < 0 ? normal : -normal; // face forward
        rec.mat = m;
        return rec;
    }
};
