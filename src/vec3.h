#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>

struct Vec3 {
    double x, y, z;
    Vec3(): x(0), y(0), z(0) {}
    Vec3(double x_, double y_, double z_): x(x_), y(y_), z(z_) {}

    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3& operator+=(const Vec3& v){ x+=v.x; y+=v.y; z+=v.z; return *this; }
    Vec3& operator-=(const Vec3& v){ x-=v.x; y-=v.y; z-=v.z; return *this; }
    Vec3& operator*=(double s){ x*=s; y*=s; z*=s; return *this; }
    Vec3& operator/=(double s){ return *this *= (1.0/s); }

    double length() const { return std::sqrt(x*x + y*y + z*z); }
    double length2() const { return x*x + y*y + z*z; }
};

inline Vec3 operator+(const Vec3&a, const Vec3&b){ return {a.x+b.x,a.y+b.y,a.z+b.z}; }
inline Vec3 operator-(const Vec3&a, const Vec3&b){ return {a.x-b.x,a.y-b.y,a.z-b.z}; }
inline Vec3 operator*(const Vec3&a, const Vec3&b){ return {a.x*b.x,a.y*b.y,a.z*b.z}; }
inline Vec3 operator*(const Vec3&a, double s){ return {a.x*s,a.y*s,a.z*s}; }
inline Vec3 operator*(double s, const Vec3&a){ return a*s; }
inline Vec3 operator/(const Vec3&a, double s){ return {a.x/s,a.y/s,a.z/s}; }

inline double dot(const Vec3&a, const Vec3&b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(const Vec3&a, const Vec3&b){
    return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}
inline Vec3 normalize(const Vec3&v){
    double len = v.length();
    return len>0 ? v/len : v;
}
inline Vec3 clamp01(const Vec3&v){
    return { std::clamp(v.x,0.0,1.0), std::clamp(v.y,0.0,1.0), std::clamp(v.z,0.0,1.0) };
}
