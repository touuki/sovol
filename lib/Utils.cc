#include "Utils.hh"
#include <ctime>

std::default_random_engine Utils::e(time(NULL));
std::uniform_real_distribution<double> Utils::dist(0., 1.);

double Utils::random(double min, double max) {
    return min + (max - min) * dist(e);
};

// See http://mathworld.wolfram.com/SpherePointPicking.html
Vector3<double> Utils::randomOnSphere() {
    double theta = random(0., 2 * M_PI);
    double u = random(-1., 1.);
    return Vector3(sqrt(1. - u * u) * cos(theta), sqrt(1. - u * u) * sin(theta),
                   u);
};

Vector3<double> Utils::velocity(const Vector3<double> &momentum, double mass) {
    return momentum / sqrt(pow(mass, 2) + momentum.square());
};

double Utils::gamma(const Vector3<double> &momentum, double mass) {
    return sqrt(1. + momentum.square() / pow(mass, 2));
};

Vector3<double> Utils::momentum(const Vector3<double> &velocity, double mass) {
    return mass / sqrt(1. - velocity.square()) * velocity;
};

Vector3<double> Utils::lorentzForce(double charge,
                                    const Vector3<double> &velocity,
                                    const EMField &em) {
    return charge * (em.e + velocity.cross(em.b));
};

double Utils::kineticEnergy(const Vector3<double> &momentum, double mass) {
    return sqrt(pow(mass, 2) + momentum.square()) - mass;
};