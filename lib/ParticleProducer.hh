#ifndef _SOVOL_PARTICLEPRODUCER_HH
#define _SOVOL_PARTICLEPRODUCER_HH 1

#include <random>

#include "Distribution.hh"
#include "FactoryHelper.hh"
#include "Particle.hh"
#include "Rotator.hh"
#include "Vector3.hh"

class ParticleProducer {
 public:
  virtual std::shared_ptr<Particle> createParticle() = 0;
  virtual ~ParticleProducer();
};

DEFINE_FACTORY(ParticleProducer)

class BeamParticleProducer : public ParticleProducer {
 private:
  std::string className;
  double charge;
  double mass;
  double width;
  double length;
  double kinetic_energy;
  double energy_spread;
  double angular_divergence;
  Vector3<double> translation;
  IntrinsicRotator<double> rotator;

  Distribution<double> kinetic_energy_dist;

  std::normal_distribution<double> position_x_dist;
  std::normal_distribution<double> position_y_dist;
  std::uniform_real_distribution<double> position_z_dist;

  std::normal_distribution<double> momentum_theta_x_dist;
  std::normal_distribution<double> momentum_theta_y_dist;

 public:
  BeamParticleProducer();
  BeamParticleProducer(
      const std::string &_className,
      const Vector3<double> &_translation = Vector3<double>::zero,
      double _charge = -1., double _mass = 1., double _width = 0.,
      double _length = 0., double _kinetic_energy = 0.,
      double _energy_spread = 0., double _angular_divergence = 0.,
      double _polar_angle = 0., double _azimuthal_angle = 0.);
  std::shared_ptr<Particle> createParticle() override;
};

#endif