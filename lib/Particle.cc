#include "Particle.hh"

#include <cmath>

REGISTER_MULTITON(Particle, Particle)

Particle::Particle() : charge(-1.), mass(1.){};

Particle::Particle(const Vector3<double> &_position,
                   const Vector3<double> &_momentum, double _charge,
                   double _mass)
    : position(_position), momentum(_momentum), charge(_charge), mass(_mass){};

Particle::~Particle(){};

void Particle::translate(const Vector3<double> &translation) {
  position += translation;
}