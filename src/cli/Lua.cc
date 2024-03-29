#include "Lua.hh"

#include "utils.hh"

std::string Lua::path;
std::map<int, std::string> Lua::args;

Lua::Lua(const std::string& _path, const std::map<int, std::string>& _args) {
  L = luaL_newstate();
  if (L == nullptr) {
    throw std::runtime_error("Lua error: Failed to new state.");
  }
  luaL_openlibs(L);
  lua_register(L, "C_random", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    double r = utils::random(n > 0 ? lua_tonumber(L, 1) : 0.,
                             n > 1 ? lua_tonumber(L, 2) : 1.);
    Lua lua(L);
    lua.push(r);
    lua.L = nullptr;
    return 1;
  });
  lua_register(L, "C_normal_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    double r = utils::normal_distribution(n > 0 ? lua_tonumber(L, 1) : 0.,
                                          n > 1 ? lua_tonumber(L, 2) : 1.);
    Lua lua(L);
    lua.push(r);
    lua.L = nullptr;
    return 1;
  });
  lua_register(L, "C_sphere_uniform_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    Vector3<double> r =
        utils::sphere_uniform_distribution(n > 0 ? lua_tonumber(L, 1) : 1.);
    Lua lua(L);
    lua.push(r);
    lua.L = nullptr;
    return 1;
  });
  lua_register(L, "C_fisher_distribution", [](lua_State* L) -> int {
    int n = lua_gettop(L);
    if (n < 1) {
      throw std::invalid_argument(
          "C_fisher_distribution expect at least one argument.");
    }
    Vector3<double> r = utils::fisher_distribution(
        lua_tonumber(L, 1), n > 1 ? lua_tonumber(L, 2) : 1.);
    Lua lua(L);
    lua.push(r);
    lua.L = nullptr;
    return 1;
  });
  setGlobal("arg", _args);
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