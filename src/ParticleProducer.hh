#ifndef _SOVOL_PARTICLEPRODUCER_HH
#define _SOVOL_PARTICLEPRODUCER_HH 1

#include "ParticleShifter.hh"

class ParticleProducer {
  Particle particle;
  std::vector<std::shared_ptr<ParticleShifter>> shifters;

 public:
#ifdef __EMSCRIPTEN__
  ParticleProducer(emscripten::val v) : particle(v["particle"]) {
    emscripten::val val_shifters = v["shifters"];
    if (!val_shifters.isUndefined()) {
      for (int i = 0; i < val_shifters["length"].as<int>(); i++) {
        shifters.push_back(ParticleShifter::createObject(val_shifters[i]));
      }
    }
  };
#else
  ParticleProducer(Lua &lua)
      : particle(lua.getField<Particle>("particle")),
        shifters(lua.getField("shifters", shifters)){};
#endif
  Particle operator()() const {
    Particle p = particle;
    p.resetOpticalDepth();
    for (auto &&shifter : shifters) {
      shifter->operator()(p);
    }
    return p;
  };
};

#endif