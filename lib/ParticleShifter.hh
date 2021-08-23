#ifndef _SOVOL_PARTICLESHIFTER_HH
#define _SOVOL_PARTICLESHIFTER_HH 1

#include "Particle.hh"

class ParticleShifter {
 public:
  virtual ~ParticleShifter(){};
  virtual void operator()(Particle &) const = 0;
};

DEFINE_FACTORY(ParticleShifter)

#endif