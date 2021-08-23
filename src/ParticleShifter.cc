#include "ParticleShifter.hh"

#include "Rotator.hh"

class CustomParticleShifter : public ParticleShifter {
#ifdef __EMSCRIPTEN__
 private:
  emscripten::val func;

 public:
  CustomParticleShifter(emscripten::val v) : func(v["func"]){};
  void operator()(Particle &p) const override {
    emscripten::val result = func(p.toEmscriptenVal());
    if (!result["position"].isUndefined()) {
      p.position = Vector3<double>(result["position"]);
    }
    if (!result["momentum"].isUndefined()) {
      p.momentum = Vector3<double>(result["momentum"]);
    }
    if (!result["polarization"].isUndefined()) {
      p.polarization = Vector3<double>(result["polarization"]);
    }
  };
  ~CustomParticleShifter(){};
#else
 private:
  std::string global_function_name;

 public:
  CustomParticleShifter(Lua &lua)
      : global_function_name(
            lua.getField<std::string>("global_function_name")){};

  void operator()(Particle &p) const override {
    Lua &lua = Lua::getInstance();
    lua.visitGlobal(global_function_name.c_str());
    lua.call(1, p);
    p.position = lua.getField("position", p.position);
    p.momentum = lua.getField("momentum", p.momentum);
    p.polarization = lua.getField("polarization", p.polarization);
    lua.leaveTable();
  };
#endif
};

REGISTER_MULTITON(ParticleShifter, CustomParticleShifter)

class ParticleTranslator : public ParticleShifter {
 private:
  Vector3<double> displacement;

 public:
  ParticleTranslator(const Vector3<double> &_displacement)
      : displacement(_displacement){};
#ifdef __EMSCRIPTEN__
  ParticleTranslator(emscripten::val v) : displacement(v["displacement"]){};
#else
  ParticleTranslator(Lua &lua)
      : displacement(lua.getField<Vector3<double> >("displacement")) {
    lua.visitField("displacement");
    displacement = Vector3<double>(lua);
    lua.leaveTable();
  };
#endif
  void operator()(Particle &p) const override { p.position += displacement; };
};

REGISTER_MULTITON(ParticleShifter, ParticleTranslator)

class ParticleRotator : public ParticleShifter {
 private:
  Vector3<double> center;
  std::shared_ptr<Rotator> rotator;
  bool affect_position;
  bool affect_momentum;
  bool affect_polarization;

 public:
  ParticleRotator(const Vector3<double> &_center,
                  const std::shared_ptr<Rotator> &_rotator,
                  bool _affect_position = true,
                  bool _affect_momentum = true,
                  bool _affect_polarization = true)
      : center(_center),
        rotator(_rotator),
        affect_position(_affect_position),
        affect_momentum(_affect_momentum),
        affect_polarization(_affect_polarization){};
#ifdef __EMSCRIPTEN__
  ParticleRotator(emscripten::val v)
      : center(v["center"].isUndefined() ? Vector3<double>()
                                         : Vector3<double>(v["center"])),
        rotator(RotatorFactory::createObject(v["rotator"])),
        affect_position(v["affect_position"].isUndefined()
                             ? true
                             : v["affect_position"].as<bool>()),
        affect_momentum(v["affect_momentum"].isUndefined()
                             ? true
                             : v["affect_momentum"].as<bool>()),
        affect_polarization(v["affect_polarization"].isUndefined()
                                 ? true
                                 : v["affect_polarization"].as<bool>()){};
#else
  ParticleRotator(Lua &lua)
      : center(lua.getField("center", Vector3<double>())),
        affect_position(lua.getField("affect_position", true)),
        affect_momentum(lua.getField("affect_momentum", true)),
        affect_polarization(lua.getField("affect_polarization", true)) {
    lua.visitField("rotator");
    rotator = RotatorFactory::createObject(lua);
    lua.leaveTable();
  };
#endif
  void operator()(Particle &p) const override {
    if (affect_position) {
      p.position = center + rotator->operator()(p.position - center);
    }

    if (affect_momentum) {
      p.momentum = rotator->operator()(p.momentum);
    }

    if (affect_polarization) {
      p.polarization = rotator->operator()(p.polarization);
    }
  };
};

REGISTER_MULTITON(ParticleShifter, ParticleRotator)
