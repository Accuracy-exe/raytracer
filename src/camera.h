#pragma once
#include "vec3.h"
#include "ray.h"

struct Camera {
    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;

    Camera(const Vec3&lookfrom, const Vec3&lookat, const Vec3&vup, double vfov_deg, double aspect){
        double theta = vfov_deg * M_PI/180.0;
        double h = std::tan(theta/2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect * viewport_height;

        Vec3 w = normalize(lookfrom - lookat);
        Vec3 u = normalize(cross(vup, w));
        Vec3 v = cross(w, u);

        origin = lookfrom;
        horizontal = u * viewport_width;
        vertical = v * viewport_height;
        lower_left_corner = origin - horizontal/2 - vertical/2 - w;
    }

    Ray get_ray(double s, double t) const {
        return Ray(origin, normalize(lower_left_corner + s*horizontal + t*vertical - origin));
    }
};
