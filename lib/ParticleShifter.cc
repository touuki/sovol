#include "ParticleShifter.hh"

#include "Rotator.hh"

class ParticleCustomShifter : public ParticleShifter {
#ifdef __EMSCRIPTEN__
 private:
  emscripten::val func;

 public:
  ParticleCustomShifter(emscripten::val v) : func(v["func"]){};
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
  ~ParticleCustomShifter(){};
#else
 private:
  std::string global_function_name;

 public:
  ParticleCustomShifter(Lua &lua)
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
  bool disable_position;
  bool disable_momentum;
  bool disable_polarization;

 public:
  ParticleRotator(const Vector3<double> &_center,
                  const std::shared_ptr<Rotator> &_rotator,
                  bool _disable_position = false,
                  bool _disable_momentum = false,
                  bool _disable_polarization = false)
      : center(_center),
        rotator(_rotator),
        disable_position(_disable_position),
        disable_momentum(_disable_momentum),
        disable_polarization(_disable_polarization){};
#ifdef __EMSCRIPTEN__
  ParticleRotator(emscripten::val v)
      : center(v["center"].isUndefined() ? Vector3<double>()
                                         : Vector3<double>(v["center"])),
        rotator(RotatorFactory::createObject(v["rotator"])),
        disable_position(v["disable_position"].isUndefined()
                             ? false
                             : v["disable_position"].as<bool>()),
        disable_momentum(v["disable_momentum"].isUndefined()
                             ? false
                             : v["disable_momentum"].as<bool>()),
        disable_polarization(v["disable_polarization"].isUndefined()
                                 ? false
                                 : v["disable_polarization"].as<bool>()){};
#else
  ParticleRotator(Lua &lua)
      : center(lua.getField("center", Vector3<double>())),
        disable_position(lua.getField("disable_position", false)),
        disable_momentum(lua.getField("disable_momentum", false)),
        disable_polarization(lua.getField("disable_polarization", false)) {
    lua.visitField("rotator");
    rotator = RotatorFactory::createObject(lua);
    lua.leaveTable();
  };
#endif
  void operator()(Particle &p) const override {
    if (!disable_position) {
      p.position = center + rotator->operator()(p.position - center);
    }

    if (!disable_momentum) {
      p.momentum = rotator->operator()(p.momentum);
    }

    if (!disable_polarization) {
      p.polarization = rotator->operator()(p.polarization);
    }
  };
};

REGISTER_MULTITON(ParticleShifter, ParticleRotator)
