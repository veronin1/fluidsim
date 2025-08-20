#pragma once

#include "liquid.hpp"
#include "vec3.hpp"
#include "vector_math.hpp"

int navier();
void applyForces(float timeStep, Liquid& fluid);
void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep);
void advectDensity(Grid3D& grid, Liquid& fluid, float timeStep);
