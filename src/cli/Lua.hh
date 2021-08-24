#ifndef _SOVOL_LUA_HH
#define _SOVOL_LUA_HH 1

#include <lua.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Lua {
 private:
  lua_State* L;
  Lua(lua_State* _L) : L(_L){};

  void push(const char* s) { lua_pushstring(L, s); };
  void push(const std::string& s) { lua_pushstring(L, s.c_str()); };
  void push(bool b) { lua_pushboolean(L, b); };
  template <typename T,
            std::enable_if_t<std::is_same_v<decltype(std::declval<T>().luaPush(
                                                std::declval<Lua&>())),
                                            void>,
                             bool> = true>
  void push(const T& v) {
    v.luaPush(*this);
  };
  template <typename Integer,
            std::enable_if_t<std::is_integral_v<Integer> &&
                                 !std::is_same_v<Integer, bool>,
                             bool> = true>
  void push(Integer v) {
    lua_pushinteger(L, v);
  };
  template <typename Floating,
            std::enable_if_t<std::is_floating_point_v<Floating>, bool> = true>
  void push(Floating v) {
    lua_pushnumber(L, v);
  };

  template <typename T,
            std::enable_if_t<
                std::is_same_v<T, std::vector<typename T::value_type,
                                              typename T::allocator_type>>,
                bool> = true>
  T as() {
    T vector;
    int type, i = 1;
    while ((type = visitField(i++)) != LUA_TNIL) {
      vector.push_back(pop<typename T::value_type>(type));
    }
    lua_pop(L, 1);  // pop nil
    return vector;
  };
  template <typename T,
            std::enable_if_t<
                std::is_same_v<decltype(T::element_type::createObject(
                                   std::declval<Lua&>())),
                               std::shared_ptr<typename T::element_type>>,
                bool> = true>
  T as() {
    return T::element_type::createObject(*this);
  };
  template <typename T,
            std::enable_if_t<std::is_constructible_v<T, Lua&>, bool> = true>
  T as() {
    return T(*this);
  };
  template <typename Bool,
            std::enable_if_t<std::is_same_v<Bool, bool>, bool> = true>
  Bool as() {
    return lua_toboolean(L, -1);
  };
  template <typename Integer,
            std::enable_if_t<std::is_integral_v<Integer> &&
                                 !std::is_same_v<Integer, bool>,
                             bool> = true>
  Integer as() {
    int isnum;
    Integer r = lua_tointegerx(L, -1, &isnum);
    if (!isnum) {
      throw std::runtime_error("Lua error: Can't convert to integral.");
    }
    return r;
  };
  template <typename Floating,
            std::enable_if_t<std::is_floating_point_v<Floating>, bool> = true>
  Floating as() {
    int isnum;
    Floating r = lua_tonumberx(L, -1, &isnum);
    if (!isnum) {
      throw std::runtime_error("Lua error: Can't convert to number.");
    }
    return r;
  }
  template <typename String,
            std::enable_if_t<std::is_same_v<String, std::string>, bool> = true>
  std::string as() {
    const char* r = lua_tostring(L, -1);
    if (!r) {
      throw std::runtime_error("Lua error: Can't convert to string.");
    }
    return std::string(r);
  }

  template <typename Value>
  Value pop(int type, const Value& defaultValue) {
    if (type == LUA_TNIL) {
      lua_pop(L, 1);
      return defaultValue;
    } else {
      return pop<Value>(type);
    }
  };
  template <typename Value>
  Value pop(int type) {
    // if (type == LUA_TFUNCTION) {
    //   call(1);
    // }
    Value r = as<Value>();
    lua_pop(L, 1);
    return r;
  };

 public:
  Lua(const Lua&) = delete;
  Lua& operator=(const Lua&) = delete;
  Lua(const std::string& _path);
  ~Lua() {
    if (L != nullptr) {
      lua_close(L);
      L = nullptr;
    }
  };
  template <typename... Args>
  void call(int nresults, Args&&... args) {
    if (!lua_isfunction(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a function");
    }
    std::tuple<Args...> tuple(std::forward<Args>(args)...);
    std::apply([this](Args... args) { (push(args), ...); }, tuple);
    int ret = lua_pcall(L, sizeof...(Args), nresults, 0);
    if (ret) {
      std::stringstream ss;
      switch (ret) {
        case LUA_ERRMEM:
          throw std::runtime_error("Lua memory allocation error.");
        case LUA_ERRRUN:
          ss << "Lua runtime error: ";
          break;
        case LUA_ERRERR:
          ss << "Lua error while running the message handler: ";
          break;
      }
      ss << as<std::string>();
      throw std::runtime_error(ss.str());
    }
  }
  template <typename Value, typename Key>
  void setField(const Key& key, const Value& value) {
    if (!lua_istable(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    push(key);
    push(value);
    lua_settable(L, -3);
  }
  template <typename Value, typename Key>
  Value getField(const Key& key, const Value& defaultValue) {
    int type = visitField(key);
    return pop<Value>(type, defaultValue);
  };
  template <typename Value, typename Key>
  Value getField(const Key& key) {
    int type = visitField(key);
    return pop<Value>(type);
  };
  template <typename Value>
  Value getGlobal(const char* name, const Value& defaultValue) {
    int type = visitGlobal(name);
    return pop<Value>(type, defaultValue);
  };
  template <typename Value>
  Value getGlobal(const char* name) {
    int type = visitGlobal(name);
    return pop<Value>(type);
  };
  int visitNext() {
    if (!lua_istable(L, -2)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    return lua_next(L, -2);
  }
  template <typename Key>
  int visitField(const Key& key) {
    if (!lua_istable(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    push(key);
    return lua_gettable(L, -2);
  }
  int visitGlobal(const char* name) { return lua_getglobal(L, name); }
  void startTraverse() {
    if (!lua_istable(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    lua_pushnil(L);
  }
  void createTable(int narr = 0, int nrec = 0) {
    lua_createtable(L, narr, nrec);
  }
  void leaveTable() {
    if (!lua_istable(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    lua_pop(L, 1);
  }
  static std::string path;
  static Lua& getInstance() {
    thread_local std::unique_ptr<Lua> instance = std::make_unique<Lua>(path);
    return *instance;
  }
};

#endif
