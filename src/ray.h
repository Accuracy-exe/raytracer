#pragma once
#include "vec3.h"

struct Ray {
    Vec3 origin;
    Vec3 dir;
    Ray() {}
    Ray(const Vec3&o, const Vec3&d): origin(o), dir(d) {}
    Vec3 at(double t) const { return origin + dir * t; }
};
