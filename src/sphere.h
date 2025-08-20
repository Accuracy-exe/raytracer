#pragma once
#include "hittable.h"

struct Sphere : public Hittable {
    Vec3 center;
    double radius;
    Material mat;

    Sphere(const Vec3&c, double r, const Material&m): center(c), radius(r), mat(m){}

    std::optional<HitRecord> hit(const Ray& r, double tmin, double tmax) const override {
        Vec3 oc = r.origin - center;
        double a = dot(r.dir, r.dir);
        double b = 2.0 * dot(oc, r.dir);
        double c = dot(oc, oc) - radius*radius;
        double disc = b*b - 4*a*c;
        if (disc < 0) return std::nullopt;
        double sqrtd = std::sqrt(disc);

        auto try_t = [&](double t)->std::optional<HitRecord>{
            if (t < tmin || t > tmax) return std::nullopt;
            HitRecord rec;
            rec.t = t;
            rec.p = r.at(t);
            rec.normal = normalize(rec.p - center);
            rec.mat = mat;
            return rec;
        };

        double t1 = (-b - sqrtd)/(2*a);
        if (auto h = try_t(t1)) return h;
        double t2 = (-b + sqrtd)/(2*a);
        if (auto h = try_t(t2)) return h;

        return std::nullopt;
    }
};
