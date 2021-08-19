#ifndef _SOVOL_FIELD_HH
#define _SOVOL_FIELD_HH 1

#include "Vector3.hh"

#define C_FUNC_P(func) double (*func)(double, double, double, double)

template <typename T>
class EMField {
 public:
  Vector3<T> E;
  Vector3<T> B;
  EMField(const Vector3<T> &_E, const Vector3<T> &_B) : E(_E), B(_B){};
#ifdef __EMSCRIPTEN__
  EMField(val v) : E(v["E"]), B(v["B"]){};
#else
  EMField(lua_State *L) {
    lua_getfield(L, -1, "E");
    E = Vector3(L);
    lua_pop(L, 1);
    lua_getfield(L, -1, "B");
    B = Vector3(L);
    lua_pop(L, 1);
  };
#endif
};

class Field {
 public:
  virtual ~Field(){};
  virtual EMField<double> operator()(const Vector3<double> &pos,
                                     double time) const = 0;
};

DEFINE_FACTORY(Field)

class CustomField : public Field {
#ifdef __EMSCRIPTEN__
 private:
  C_FUNC_P(Ex);
  C_FUNC_P(Ey);
  C_FUNC_P(Ez);
  C_FUNC_P(Bx);
  C_FUNC_P(By);
  C_FUNC_P(Bz);

 public:
  CustomField(val v)
      : Ex(v["Ex"].as<C_FUNC_P>()),
        Ey(v["Ey"].as<C_FUNC_P>()),
        Ez(v["Ez"].as<C_FUNC_P>()),
        Bx(v["Bx"].as<C_FUNC_P>()),
        By(v["By"].as<C_FUNC_P>()),
        Bz(v["Bz"].as<C_FUNC_P>()){};
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    double x = pos.x;
    double y = pos.y;
    double z = pos.z;
    return EMField{Vector3(Ex(x, y, z, t), Ey(x, y, z, t), Ez(x, y, z, t)),
                   Vector3(Bx(x, y, z, t), By(x, y, z, t), Bz(x, y, z, t))};
  };
#else
 private:
  std::string function_name;

 public:
  CustomField(lua_State *L) {
    lua_getfield(L, -1, "function_name");
    function_name = lua_tostring(L, -1);
    lua_pop(L, 1);
  };
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    lua_State *L = Lua::getState();
    lua_getglobal(L, function_name.c_str());
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);
    lua_pushnumber(L, t);
    int ret = lua_pcall(L, 4, 1, 0);
    if (ret) {
      /* error */
    }
    EMField<double> em(L);
    lua_pop(L, 1);
    return em;
  };
#endif
};

REGISTER_MULTITON(Field, CustomField)

#endif