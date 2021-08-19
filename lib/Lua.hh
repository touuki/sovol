#ifndef _SOVOL_LUA_HH
#define _SOVOL_LUA_HH 1

#include <lua.hpp>
#include <memory>
#include <string>

class Lua {
 private:
  lua_State* L;
  Lua() {
    L = luaL_newstate();
    if (L == nullptr) {
      /* error */
    }
    int ret = luaL_loadfile(L, path.c_str());
    if (ret) {
      /* error */
    }
    ret = lua_pcall(L, 0, 0, 0);
    if (ret) {
      /* error */
    }
  };

 public:
  static std::string path;
  ~Lua() {
    if (L != nullptr) {
      lua_close(L);
      L = nullptr;
    }
  };
  static lua_State* getState() {
    thread_local std::unique_ptr<Lua> instance = std::make_unique<Lua>();
    return instance->L;
  }
};

#endif
