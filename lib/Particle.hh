#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "Field.hh"
#include <vector>

class Particle {
  public:
    virtual ~Particle();
    Vector3<double> position;
    Vector3<double> momentum;
    double charge;
    double mass;

    Particle(const Vector3<double> &_position, const Vector3<double> &_momentum,
             double _charge, double _mass);
    virtual void nextStep(const Field &, double time, double dt) = 0;
    void rotate(double polar_angle, double azimuthal_angle);
    void translate(const Vector3<double> &translation);
};

class RungeKuttaParticle : public Particle {
  public:
    RungeKuttaParticle(const Vector3<double> &_position,
                       const Vector3<double> &_momentum, double _charge,
                       double _mass);
    void nextStep(const Field &, double time, double dt) override;
};

class LeapfrogParticle : public Particle {
  public:
    LeapfrogParticle(const Vector3<double> &_position,
                     const Vector3<double> &_momentum, double _charge,
                     double _mass);
    void nextStep(const Field &, double time, double dt) override;
};
#endif