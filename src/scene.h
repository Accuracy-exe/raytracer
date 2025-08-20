#pragma once
#include "hittable.h"
#include "camera.h"
#include <limits>
#include <optional>
#include <random>
#include <atomic>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>

struct Scene {
    std::vector<HittablePtr> objects;
    std::vector<Light> lights;
    Vec3 background{0.7, 0.8, 1.0}; // sky color
    Vec3 ambient{0.05, 0.05, 0.05}; // ambient term

    void add(const HittablePtr& obj) { objects.push_back(obj); }
    void add_light(const Light& l) { lights.push_back(l); }

    std::optional<HitRecord> trace_closest(const Ray& r, double tmin, double tmax) const {
        std::optional<HitRecord> closest;
        double closest_t = tmax;
        for (const auto& obj : objects) {
            auto hit = obj->hit(r, tmin, closest_t);
            if (hit) {
                closest_t = hit->t;
                closest = hit;
            }
        }
        return closest;
    }
};

// Utility: reflect a vector
inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2.0 * dot(v, n) * n;
}

struct RendererSettings {
    int width = 800;
    int height = 450;
    int samples_per_pixel = 4;
    int max_depth = 3; // reflection bounces
    bool enable_reflections = true;
    int threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4;
};

// Whitted-style shading with hard shadows + Phong highlights
inline Vec3 shade(const Scene& scene, const Ray& r, int depth, std::mt19937_64& rng) {
    if (depth <= 0) return {0,0,0};

    auto hit = scene.trace_closest(r, 1e-4, std::numeric_limits<double>::infinity());
    if (!hit) {
        return scene.background;
    }

    const auto& rec = *hit;
    Vec3 N = rec.normal;
    Vec3 V = normalize(-r.dir);

    Vec3 color = scene.ambient * rec.mat.albedo;

    // Lights
    for (const auto& Lsrc : scene.lights) {
        Vec3 Ldir = Lsrc.position - rec.p;
        double dist2 = Ldir.length2();
        double dist = std::sqrt(dist2);
        Ldir /= dist;

        // Shadow ray
        Ray shadowRay(rec.p + N*1e-4, Ldir);
        auto occluder = scene.trace_closest(shadowRay, 1e-4, dist - 1e-4);
        if (occluder) continue; // in shadow

        double nDotL = std::max(0.0, dot(N, Ldir));
        Vec3 diffuse = rec.mat.kd * nDotL * (rec.mat.albedo * (Lsrc.color * (Lsrc.intensity / dist2)));

        Vec3 R = reflect(-Ldir, N);
        double rDotV = std::max(0.0, dot(R, V));
        Vec3 specular = rec.mat.ks * std::pow(rDotV, rec.mat.shininess) * (Lsrc.color * (Lsrc.intensity / dist2));

        color += diffuse + specular;
    }

    // Reflection (optional)
    if (rec.mat.reflectivity > 0.0) {
        Vec3 Rdir = reflect(-V, N);
        Ray rr(rec.p + N*1e-4, normalize(Rdir));
        Vec3 rcol = shade(scene, rr, depth - 1, rng);
        color = (1.0 - rec.mat.reflectivity) * color + rec.mat.reflectivity * rcol;
    }

    return color;
}

inline double rand01(std::mt19937_64& rng) {
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

// Multithreaded render
inline void render(const Scene& scene, const RendererSettings& cfg, const Camera& cam, const std::string& outFile) {
    const int W = cfg.width;
    const int H = cfg.height;
    const int S = cfg.samples_per_pixel;

    std::vector<Vec3> framebuffer(W * H);
    std::atomic<int> next_row{0};

    auto worker = [&](int thread_id){
        std::mt19937_64 rng(std::hash<std::thread::id>{}(std::this_thread::get_id())
                            ^ (std::mt19937_64::result_type)std::chrono::high_resolution_clock::now().time_since_epoch().count());

        while (true) {
            int j = next_row.fetch_add(1);
            if (j >= H) break;

            for (int i = 0; i < W; ++i) {
                Vec3 col{0,0,0};
                for (int s = 0; s < S; ++s) {
                    double u = (i + rand01(rng)) / (W - 1);
                    double v = (j + rand01(rng)) / (H - 1);
                    Ray r = cam.get_ray(u, 1.0 - v); // flip vertical for image coordinates
                    col += shade(scene, r, cfg.max_depth, rng);
                }
                col /= static_cast<double>(S);

                // Gamma correction (gamma=2.2 approx by sqrt)
                col = { std::sqrt(std::clamp(col.x, 0.0, 1.0)),
                        std::sqrt(std::clamp(col.y, 0.0, 1.0)),
                        std::sqrt(std::clamp(col.z, 0.0, 1.0)) };

                framebuffer[j*W + i] = col;
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(cfg.threads);
    for (int t = 0; t < cfg.threads; ++t) threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    // Write PPM
    std::ofstream out(outFile, std::ios::binary);
    out << "P6\n" << W << " " << H << "\n255\n";
    for (int j = 0; j < H; ++j) {
        for (int i = 0; i < W; ++i) {
            Vec3 c = clamp01(framebuffer[j*W + i]);
            unsigned char r = static_cast<unsigned char>(255.999 * c.x);
            unsigned char g = static_cast<unsigned char>(255.999 * c.y);
            unsigned char b = static_cast<unsigned char>(255.999 * c.z);
            out.put(r); out.put(g); out.put(b);
        }
    }
}
