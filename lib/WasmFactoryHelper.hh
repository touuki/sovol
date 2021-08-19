#ifndef _SOVOL_WASMFACTORYHELPER_HH
#define _SOVOL_WASMFACTORYHELPER_HH 1

#include <map>
#include <memory>
#include <string>

#define DEFINE_FACTORY(base_name)                                          \
  typedef std::shared_ptr<base_name> (*base_name##Constructor)(val v);     \
  class base_name##Factory {                                               \
   public:                                                                 \
    static void registerClass(const std::string &className,                \
                              base_name##Constructor constructor) {        \
      constructors()[className] = constructor;                             \
    }                                                                      \
                                                                           \
    static std::shared_ptr<base_name> createObject(val v) {                \
      std::string className = v["name"].as<std::string>();                 \
      base_name##Constructor constructor = nullptr;                        \
      if (constructors().find(className) != constructors().end())          \
        constructor = constructors().find(className)->second;              \
      if (constructor == nullptr) {                                        \
        std::cerr << "ERROR: Create " #base_name " failed. Class "         \
                  << className << " is not found." << std::endl;           \
        return nullptr;                                                    \
      }                                                                    \
      return (*constructor)(v);                                            \
    }                                                                      \
                                                                           \
   private:                                                                \
    static std::map<std::string, base_name##Constructor> &constructors() { \
      static std::map<std::string, base_name##Constructor> instance;       \
      return instance;                                                     \
    }                                                                      \
  };

#define REGISTER_MULTITON(base_name, class_name)                           \
  class class_name##Helper {                                               \
   public:                                                                 \
    class_name##Helper() {                                                 \
      base_name##Factory::registerClass(#class_name,                       \
                                        class_name##Helper::creatObjFunc); \
    }                                                                      \
    static std::shared_ptr<base_name> creatObjFunc(val v) {                \
      return std::make_shared<class_name>(v);                              \
    }                                                                      \
  };                                                                       \
  class_name##Helper class_name##helper;

#define REGISTER_SINGLETON(base_name, class_name)                          \
  class class_name##Helper {                                               \
   public:                                                                 \
    class_name##Helper() {                                                 \
      base_name##Factory::registerClass(#class_name,                       \
                                        class_name##Helper::creatObjFunc); \
    }                                                                      \
    static std::shared_ptr<base_name> creatObjFunc(val v) {                \
      if (instance == nullptr) instance = std::make_shared<class_name>();  \
      return instance;                                                     \
    }                                                                      \
                                                                           \
   private:                                                                \
    static std::shared_ptr<base_name> instance;                            \
  };                                                                       \
  std::shared_ptr<base_name> class_name##Helper::instance = nullptr;       \
  class_name##Helper class_name##helper;

#endif