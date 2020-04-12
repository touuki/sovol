#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "ParticleFactory.hh"
#include "Vector3.hh"
#include <vector>

class ParticleFactory;

class Particle {
  public:
    virtual ~Particle();
    Vector3<double> position;
    Vector3<double> momentum;
    double charge;
    double mass;

    Particle(ParticleFactory *);
    Particle(const Vector3<double> &_position, const Vector3<double> &_momentum,
             double _charge, double _mass);
    void rotate(double polar_angle, double azimuthal_angle);
    void translate(const Vector3<double> &translation);
};
#endif