#ifndef _SOVOL_LUA_HH
#define _SOVOL_LUA_HH 1

#include <lua.hpp>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class convert_error : public std::runtime_error {
 public:
  explicit convert_error(const std::string& __arg) _GLIBCXX_TXN_SAFE
      : std::runtime_error(__arg){};
#if __cplusplus >= 201103L
  explicit convert_error(const char* s) _GLIBCXX_TXN_SAFE
      : std::runtime_error(s){};
  convert_error(const convert_error&) = default;
  convert_error& operator=(const convert_error&) = default;
  convert_error(convert_error&&) = default;
  convert_error& operator=(convert_error&&) = default;
#endif
  virtual ~convert_error() _GLIBCXX_NOTHROW{};
};

class Lua {
 private:
  lua_State* L;
  Lua(lua_State* _L) : L(_L){};

  void push(const char* s) { lua_pushstring(L, s); };
  void push(const std::string& s) { lua_pushstring(L, s.c_str()); };
  void push(bool b) { lua_pushboolean(L, b); };
  template <
      typename T,
      std::enable_if_t<
          std::is_same_v<
              T, std::map<typename T::key_type, typename T::mapped_type,
                          typename T::key_compare, typename T::allocator_type>>,
          bool> = true>
  void push(const T& map) {
    createTable();
    for (auto&& pair : map) {
      setField(pair.first, pair.second);
    }
  };
  template <typename T,
            std::enable_if_t<
                std::is_same_v<T, std::vector<typename T::value_type,
                                              typename T::allocator_type>>,
                bool> = true>
  void push(const T& vector) {
    size_t size = vector.size();
    createTable(size, 0);
    for (size_t i = 0; i < size; i++) {
      setField(i + 1, vector[i]);
    }
  };
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

  template <
      typename T,
      std::enable_if_t<
          std::is_same_v<
              T, std::map<typename T::key_type, typename T::mapped_type,
                          typename T::key_compare, typename T::allocator_type>>,
          bool> = true>
  T as() {
    T map;
    lua_pushnil(L);
    while (lua_next(L, -2) != LUA_TNIL) {
      typename T::mapped_type&& v = pop<typename T::mapped_type>();
      map[as<typename T::key_type>()] = v;
    }
    lua_pop(L, 1);  // pop nil
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
      throw convert_error("Can't convert to integral.");
    }
    return r;
  };
  template <typename Floating,
            std::enable_if_t<std::is_floating_point_v<Floating>, bool> = true>
  Floating as() {
    int isnum;
    Floating r = lua_tonumberx(L, -1, &isnum);
    if (!isnum) {
      throw convert_error("Can't convert to number.");
    }
    return r;
  }
  template <typename String,
            std::enable_if_t<std::is_same_v<String, std::string>, bool> = true>
  std::string as() {
    const char* r = lua_tostring(L, -1);
    if (!r) {
      throw convert_error("Can't convert to string.");
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
  Lua(const std::string& _path, const std::map<int, std::string>& _args);
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
    try {
      return pop<Value>(type, defaultValue);
    } catch (const convert_error& e) {
      std::stringstream ss;
      ss << "Convert errot when get field [" << key << "]:" << e.what();
      throw std::runtime_error(ss.str());
    }
  };
  template <typename Value, typename Key>
  Value getField(const Key& key) {
    int type = visitField(key);
    try {
      return pop<Value>(type);
    } catch (const convert_error& e) {
      std::stringstream ss;
      ss << "Convert errot when get field [" << key << "]:" << e.what();
      throw std::runtime_error(ss.str());
    }
  };
  template <typename Key>
  int visitField(const Key& key) {
    if (!lua_istable(L, -1)) {
      throw std::runtime_error("Input lua script error: Not a table");
    }
    push(key);
    return lua_gettable(L, -2);
  }

  template <typename Value>
  void setGlobal(const char* name, const Value& value) {
    push(value);
    lua_setglobal(L, name);
  }
  template <typename Value>
  Value getGlobal(const char* name, const Value& defaultValue) {
    int type = visitGlobal(name);
    try {
      return pop<Value>(type, defaultValue);
    } catch (const convert_error& e) {
      std::stringstream ss;
      ss << "Convert errot when get global [" << name << "]:" << e.what();
      throw std::runtime_error(ss.str());
    }
  };
  template <typename Value>
  Value getGlobal(const char* name) {
    int type = visitGlobal(name);
    try {
      return pop<Value>(type);
    } catch (const convert_error& e) {
      std::stringstream ss;
      ss << "Convert errot when get global [" << name << "]:" << e.what();
      throw std::runtime_error(ss.str());
    }
  };
  int visitGlobal(const char* name) { return lua_getglobal(L, name); }

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
  static std::map<int, std::string> args;
  static Lua& getInstance() {
    thread_local std::unique_ptr<Lua> instance =
        std::make_unique<Lua>(path, args);
    return *instance;
  }
};

#endif
