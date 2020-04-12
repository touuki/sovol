#include "Particle.hh"
#include "ParticleFactory.hh"
#include "Utils.hh"

Particle::Particle(const Vector3<double> &_position,
                   const Vector3<double> &_momentum, double _charge,
                   double _mass)
    : position(_position), momentum(_momentum), charge(_charge), mass(_mass){};

Particle::Particle(ParticleFactory *factory)
    : position(Vector3(0., 0., 0.)), momentum(Vector3(0., 0., 0.)), charge(factory->charge),
      mass(factory->mass) {

    double Ek = factory->kinetic_energy_dist(factory->random_engine);
    while (Ek < 0.)
        Ek = factory->kinetic_energy_dist(factory->random_engine);

    double p = sqrt(pow(Ek + mass, 2) - pow(mass, 2));
    double theta_x = factory->momentum_theta_x_dist(factory->random_engine);
    double theta_y = factory->momentum_theta_y_dist(factory->random_engine);
    double theta = sqrt(pow(theta_x, 2) + pow(theta_y, 2));
    double phi = atan2(theta_y, theta_x);
    momentum = Vector3(p * sin(theta) * cos(phi),
                             p * sin(theta) * sin(phi), p * cos(theta));
    position = Vector3(factory->position_x_dist(factory->random_engine),
                             factory->position_y_dist(factory->random_engine),
                             factory->position_z_dist(factory->random_engine));

    rotate(factory->polar_angle, factory->azimuthal_angle);
    translate(factory->translation);
};

Particle::~Particle(){};

// See https://en.wikipedia.org/wiki/Euler_angles
//     https://en.wikipedia.org/wiki/Rotation_matrix
void Particle::rotate(double beta, double alpha) {
    Vector3<double> rotateX(cos(alpha) * cos(beta), -sin(alpha),
                            cos(alpha) * sin(beta));
    Vector3<double> rotateY(sin(alpha) * cos(beta), cos(alpha),
                            sin(alpha) * sin(beta));
    Vector3<double> rotateZ(-sin(beta), 0., cos(beta));
    position = Vector3(rotateX.dot(position), rotateY.dot(position),
                       rotateZ.dot(position));
    momentum = Vector3(rotateX.dot(momentum), rotateY.dot(momentum),
                       rotateZ.dot(momentum));
};

void Particle::translate(const Vector3<double> &translation) {
    position += translation;
}

REGISTER_PARTICLE(Particle)