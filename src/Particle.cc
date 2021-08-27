#include "Particle.hh"

#include "utils.hh"

#ifdef __EMSCRIPTEN__
Particle::Particle(emscripten::val v)
    : type(v["type"].isUndefined()
               ? normal
               : static_cast<ParticleType>(v["type"].as<int>())),
      position(v["position"].isUndefined() ? Vector3<double>::zero
                                           : Vector3<double>(v["position"])),
      momentum(v["momentum"].isUndefined() ? Vector3<double>::zero
                                           : Vector3<double>(v["momentum"])),
      polarization(v["polarization"].isUndefined()
                       ? Vector3<double>::zero
                       : Vector3<double>(v["polarization"])) {
  if (type == electron) {
    mass = 1.;
    charge = -1.;
  } else {
    mass = v["mass"].as<double>();
    charge = v["charge"].as<double>();
  }
};
emscripten::val Particle::toEmscriptenVal() const {
  emscripten::val v = emscripten::val::object();
  v.set("type", static_cast<int>(type));
  v.set("mass", mass);
  v.set("charge", charge);
  v.set("position", position.toEmscriptenVal());
  v.set("momentum", momentum.toEmscriptenVal());
  v.set("polarization", polarization.toEmscriptenVal());
  v.set("em_field", em_field.toEmscriptenVal());
  v.set("optical_depth", optical_depth);
  return v;
};
#else
Particle::Particle(Lua &lua)
    : type(static_cast<ParticleType>(lua.getField("type", 0))),
      position(lua.getField("position", Vector3<double>::zero)),
      momentum(lua.getField("momentum", Vector3<double>::zero)),
      polarization(lua.getField("polarization", Vector3<double>::zero)) {
  if (type == electron) {
    mass = 1.;
    charge = -1.;
  } else {
    mass = lua.getField<double>("mass");
    charge = lua.getField<double>("charge");
  }
};
void Particle::luaPush(Lua &lua) const {
  lua.createTable();
  lua.setField("type", static_cast<int>(type));
  lua.setField("mass", mass);
  lua.setField("charge", charge);
  lua.setField("position", position);
  lua.setField("momentum", momentum);
  lua.setField("polarization", polarization);
  lua.setField("em_field", em_field);
  lua.setField("optical_depth", optical_depth);
};
#endif
