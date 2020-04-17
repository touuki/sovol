#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "FactoryHelper.hh"
#include "Vector3.hh"

class Particle {
  public:
    virtual ~Particle();
    Vector3<double> position;
    Vector3<double> momentum;
    double charge;
    double mass;

    Particle();
    Particle(const Vector3<double> &_position, const Vector3<double> &_momentum,
             double _charge, double _mass);
    void rotate(double polar_angle, double azimuthal_angle);
    void translate(const Vector3<double> &translation);
};

DEFINE_FACTORY(Particle)

#endif