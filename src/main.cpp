#include "scene.h"
#include "sphere.h"
#include "plane.h"
#include "camera.h"
#include <memory>
#include <iostream>

int main(int argc, char** argv){
    RendererSettings cfg;
    if (argc >= 3) {
        cfg.width  = std::max(64, std::atoi(argv[1]));
        cfg.height = std::max(64, std::atoi(argv[2]));
    }
    if (argc >= 4) {
        cfg.samples_per_pixel = std::max(1, std::atoi(argv[3]));
    }
    if (argc >= 5) {
        cfg.threads = std::max(1, std::atoi(argv[4]));
    }

    // Scene
    Scene scene;
    scene.background = {0.70, 0.85, 1.0}; // light blue sky
    scene.ambient = {0.06, 0.06, 0.08};

    // Materials
    Material red;    red.albedo={0.9,0.2,0.2}; red.kd=0.9; red.ks=0.4; red.shininess=80;
    Material green;  green.albedo={0.2,0.9,0.2}; green.kd=0.9; green.ks=0.3; green.shininess=50;
    Material blue;   blue.albedo={0.2,0.4,0.9}; blue.kd=0.85; blue.ks=0.7; blue.shininess=150; blue.reflectivity=0.2;
    Material mirror; mirror.albedo={1.0,1.0,1.0}; mirror.kd=0.0; mirror.ks=0.9; mirror.shininess=300; mirror.reflectivity=0.7;

    // Checkered plane materials (just color + diffuse)
    Material white; white.albedo={0.95,0.95,0.95}; white.kd=0.95; white.ks=0.05; white.shininess=30;
    Material black; black.albedo={0.05,0.05,0.05}; black.kd=0.95; black.ks=0.05; black.shininess=30;

    // Geometry
    // Ground plane at y=-1 with 1.5 world unit checker
    scene.add(std::make_shared<Plane>(Vec3{0,-1,0}, Vec3{0,1,0}, white, black, 1.5));

    // Spheres
    scene.add(std::make_shared<Sphere>(Vec3{-1.4, -0.0, -3.5}, 1.0, red));
    scene.add(std::make_shared<Sphere>(Vec3{ 1.6, -0.3, -4.0}, 0.7, green));
    scene.add(std::make_shared<Sphere>(Vec3{ 0.2,  0.3, -2.5}, 0.5, blue));
    scene.add(std::make_shared<Sphere>(Vec3{ 0.0, -0.2, -5.5}, 0.8, mirror));

    // Lights
    scene.add_light(Light{ Vec3{-2.5, 4.5, -1.5}, Vec3{1,1,1}, 30.0 });
    scene.add_light(Light{ Vec3{ 3.5, 3.5, -2.0}, Vec3{1,0.95,0.9}, 20.0 });

    // Camera
    double aspect = double(cfg.width) / double(cfg.height);
    Camera cam(/*lookfrom*/ Vec3{0, 1.0, 2.5},
               /*lookat*/   Vec3{0, 0.0, -3.5},
               /*vup*/      Vec3{0, 1, 0},
               /*vfov*/     45.0, aspect);

    // Output
    const std::string outFile = "out.ppm";

    auto start = std::chrono::high_resolution_clock::now();
    render(scene, cfg, cam, outFile);
    auto end = std::chrono::high_resolution_clock::now();
    double secs = std::chrono::duration<double>(end - start).count();

    std::cout << "Rendered " << cfg.width << "x" << cfg.height
              << " spp=" << cfg.samples_per_pixel
              << " threads=" << cfg.threads
              << " in " << secs << "s\n";
    std::cout << "Wrote: " << outFile << "\n";
    std::cout << "Tip: view with `magick display out.ppm` or convert: `magick convert out.ppm out.png`\n";
    return 0;
}
