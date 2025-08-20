cmake_minimum_required(VERSION 3.12)
project(RayTracerMT CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimize by default for Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(raytracer
  src/main.cpp
  src/vec3.h
  src/ray.h
  src/camera.h
  src/hittable.h
  src/sphere.h
  src/plane.h
  src/scene.h
)

# MSVC secure CRT warnings off
if(MSVC)
  target_compile_options(raytracer PRIVATE /W4 /permissive-)
else()
  target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic -O3)
endif()
