#ifndef _SOVOL_PARTICLE_HH
#define _SOVOL_PARTICLE_HH 1

#include "Field.hh"

enum ParticleType { normal, electron };

class Particle {
 public:
  ParticleType type;
  double mass, charge, opticalDepth;
  Vector3<double> position, momentum, spin;
  EMField<double> em;
  Particle() : type(electron), mass(1.), charge(-1.) { resetOpticalDepth(); };
#ifdef __EMSCRIPTEN__
  Particle(emscripten::val v)
      : type(v["type"].isUndefined()
                 ? normal
                 : static_cast<ParticleType>(v["type"].as<int>())) {
    if (type == electron) {
      mass = 1.;
      charge = -1.;
      resetOpticalDepth();
    } else {
      mass = v["mass"].as<double>();
      charge = v["charge"].as<double>();
    }

    emscripten::val vfs(std::string("function"));

    emscripten::val cv = v["position"];
    if (!cv.isUndefined()) {
      if (cv.typeOf() == vfs) {
        position = Vector3<double>(cv());
      } else {
        position = Vector3<double>(cv);
      }
    }

    cv = v["momentum"];
    if (!cv.isUndefined()) {
      if (cv.typeOf() == vfs) {
        momentum = Vector3<double>(cv());
      } else {
        momentum = Vector3<double>(cv);
      }
    }

    cv = v["spin"];
    if (!cv.isUndefined()) {
      if (cv.typeOf() == vfs) {
        spin = Vector3<double>(cv());
      } else {
        spin = Vector3<double>(cv);
      }
    }
  };
#else
  Particle(Lua &lua) {
    type = static_cast<ParticleType>(lua.getField("type", 0));
    if (type == electron) {
      mass = 1.;
      charge = -1.;
      resetOpticalDepth();
    } else {
      mass = lua.getField<double>("mass");
      charge = lua.getField<double>("charge");
    }

    int lua_type = lua.visitField("position");
    if (lua_type == LUA_TNIL) {
      lua.popNil();
    } else {
      if (lua_type == LUA_TFUNCTION) {
        lua.call(1);
      }
      position = Vector3<double>(lua);
      lua.leaveTable();
    }

    int lua_type = lua.visitField("momentum");
    if (lua_type == LUA_TNIL) {
      lua.popNil();
    } else {
      if (lua_type == LUA_TFUNCTION) {
        lua.call(1);
      }
      momentum = Vector3<double>(lua);
      lua.leaveTable();
    }

    int lua_type = lua.visitField("spin");
    if (lua_type == LUA_TNIL) {
      lua.popNil();
    } else {
      if (lua_type == LUA_TFUNCTION) {
        lua.call(1);
      }
      spin = Vector3<double>(lua);
      lua.leaveTable();
    }
  };
#endif
  void resetOpticalDepth() { opticalDepth = -std::log(1. - Utils::random()); };
};

#endif