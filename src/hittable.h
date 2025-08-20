#pragma once
#include "vec3.h"
#include "ray.h"
#include <optional>
#include <memory>
#include <vector>

struct Material {
    Vec3 albedo{1,1,1};      // base color
    double kd = 0.9;          // diffuse
    double ks = 0.5;          // specular
    double shininess = 50.0;  // Phong exponent
    double reflectivity = 0.0;// 0..1
};

struct HitRecord {
    double t;
    Vec3 p;
    Vec3 normal;
    Material mat;
};

struct Hittable {
    virtual ~Hittable() = default;
    virtual std::optional<HitRecord> hit(const Ray& r, double tmin, double tmax) const = 0;
};

using HittablePtr = std::shared_ptr<Hittable>;

struct Light {
    Vec3 position;
    Vec3 color;     // typically 1,1,1
    double intensity; // multiplier (e.g., 5.0)
};
