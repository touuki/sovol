#ifndef _SOVOL_PARTICLEFACTORY_HH
#define _SOVOL_PARTICLEFACTORY_HH 1

#include "Particle.hh"
#include "Vector3.hh"
#include <random>

class ParticleFactory {
  public:
    virtual ~ParticleFactory();
    virtual Particle *createParticle() = 0;
};

class BeamParticleFactory : public ParticleFactory {
  private:
    double charge;
    double mass;
    double width;
    double length;
    double kinetic_energy;
    double energy_spread;
    double angular_divergence;
    double polar_angle;
    double azimuthal_angle;
    Vector3<double> translation;

    std::default_random_engine random_engine;

    std::normal_distribution<double> kinetic_energy_dist;

    std::normal_distribution<double> position_x_dist;
    std::normal_distribution<double> position_y_dist;
    std::uniform_real_distribution<double> position_z_dist;

    std::normal_distribution<double> momentum_theta_x_dist;
    std::normal_distribution<double> momentum_theta_y_dist;

    Vector3<double> getMomentum();
    Vector3<double> getPosition();
    void postCreation(Particle *);
    virtual Particle *_createParticle();

  public:
    BeamParticleFactory();
    BeamParticleFactory(double _charge, double _mass, double _width = 0.,
                    double _length = 0., double _kinetic_energy = 0.,
                    double _energy_spread = 0., double _angular_divergence = 0.,
                    double _polar_angle = 0., double _azimuthal_angle = 0.,
                    double _translation_x = 0., double _translation_y = 0.,
                    double _translation_z = 0.);
    Particle *createParticle() override;
};

#endif