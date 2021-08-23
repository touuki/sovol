#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "Field.hh"
#include "Utils.hh"

enum ParticleType { normal, electron };

class Particle {
 public:
  ParticleType type;
  double mass, charge, opticalDepth;
  Vector3<double> position, momentum, polarization;
  EMField<double> em_field;
  Particle() : type(electron), mass(1.), charge(-1.) { resetOpticalDepth(); };
#ifdef __EMSCRIPTEN__
  Particle(emscripten::val v);
  emscripten::val toEmscriptenVal() const;
#else
  Particle(Lua &lua);
  void luaPush(Lua &lua) const;
#endif
  void resetOpticalDepth() { opticalDepth = -std::log(1. - Utils::random()); };
};

#endif