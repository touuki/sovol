#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "Field.hh"
#include "utils.hh"

enum ParticleType { normal, electron };

class Particle {
 public:
  ParticleType type;
  double mass, charge, optical_depth;
  Vector3<double> position, momentum, polarization;
  EMField<double> em_field;
  Particle() : type(electron), mass(1.), charge(-1.){};
#ifdef __EMSCRIPTEN__
  Particle(emscripten::val v);
  emscripten::val toEmscriptenVal() const;
#else
  Particle(Lua &lua);
  void luaPush(Lua &lua) const;
#endif
  void resetOpticalDepth() { optical_depth = -std::log(1. - utils::random()); };
};

#endif