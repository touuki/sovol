#include "Lua.hh"

#include "Utils.hh"

std::string Lua::path;

Lua::Lua(const std::string& _path) {
  L = luaL_newstate();
  if (L == nullptr) {
    throw std::runtime_error("Lua error: Failed to new state.");
  }
  luaL_openlibs(L);
  lua_register(L, "C_random", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    double r = Utils::random(n > 0 ? lua_tonumber(L, 1) : 0.,
                             n > 1 ? lua_tonumber(L, 2) : 1.);
    lua_pushnumber(L, r);
    return 1;
  });
  lua_register(L, "C_normal_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    double r = Utils::normal_distribution(n > 0 ? lua_tonumber(L, 1) : 0.,
                                          n > 1 ? lua_tonumber(L, 2) : 1.);
    lua_pushnumber(L, r);
    return 1;
  });
  lua_register(L, "C_sphere_uniform_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    Vector3<double> r =
        Utils::sphere_uniform_distribution(n > 0 ? lua_tonumber(L, 1) : 1.);
    Lua lua(L);
    r.luaPush(lua);
    lua.L = nullptr;
    return 1;
  });
  lua_register(L, "C_fisher_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    if (n < 1) {
      throw std::invalid_argument(
          "C_fisher_distribution expect at least one argument.");
    }

    Vector3<double> r = Utils::fisher_distribution(
        lua_tonumber(L, 1), n > 0 ? lua_tonumber(L, 2) : 1.);
    Lua lua(L);
    r.luaPush(lua);
    lua.L = nullptr;
    return 1;
  });
  int ret = luaL_loadfile(L, _path.c_str());
  if (ret) {
    std::stringstream ss;
    switch (ret) {
      case LUA_ERRMEM:
        throw std::runtime_error("Lua memory allocation error.");
      case LUA_ERRSYNTAX:
        ss << "Lua syntax error: ";
        break;
      case LUA_ERRFILE:
        ss << "Load lua file failed: ";
        break;
    }
    ss << as<std::string>();
    throw std::runtime_error(ss.str());
  }
  call(LUA_MULTRET);
};