#include "Particle.hh"
#include <cmath>

REGISTER_MULTITON(Particle, Particle)

Particle::Particle() : charge(-1.), mass(1.) {};

Particle::Particle(const Vector3<double> &_position,
                   const Vector3<double> &_momentum, double _charge,
                   double _mass)
    : position(_position), momentum(_momentum), charge(_charge), mass(_mass){};

Particle::~Particle(){};

// See https://en.wikipedia.org/wiki/Euler_angles
//     https://en.wikipedia.org/wiki/Rotation_matrix
void Particle::rotate(double beta, double alpha) {
    Vector3<double> rotateX(std::cos(alpha) * std::cos(beta), -std::sin(alpha),
                            std::cos(alpha) * std::sin(beta));
    Vector3<double> rotateY(std::sin(alpha) * std::cos(beta), std::cos(alpha),
                            std::sin(alpha) * std::sin(beta));
    Vector3<double> rotateZ(-std::sin(beta), 0., std::cos(beta));
    position = Vector3(rotateX.dot(position), rotateY.dot(position),
                       rotateZ.dot(position));
    momentum = Vector3(rotateX.dot(momentum), rotateY.dot(momentum),
                       rotateZ.dot(momentum));
};

void Particle::translate(const Vector3<double> &translation) {
    position += translation;
}