#ifndef _SOVOL_FIELDFACTORY_HH
#define _SOVOL_FIELDFACTORY_HH 1

#include "Field.hh"
#include <map>
#include <string>

typedef Field *(*FieldConstructor)();

class FieldFactory {
  public:
    static void registerClass(const std::string &className,
                              FieldConstructor constructor) {
        constructors()[className] = constructor;
    }

    static Field *createObject(const std::string &className) {
        FieldConstructor constructor = nullptr;

        if (constructors().find(className) != constructors().end())
            constructor = constructors().find(className)->second;

        if (constructor == nullptr) {
            std::cerr << "ERROR: Create Field failed. Class " << className
                      << " is not found." << std::endl;
            return nullptr;
        }

        return (*constructor)();
    }

  private:
    inline static std::map<std::string, FieldConstructor> &constructors() {
        static std::map<std::string, FieldConstructor> instance;
        return instance;
    }
};

#define REGISTER_FIELD(class_name)                                             \
    class class_name##Helper {                                                 \
      public:                                                                  \
        class_name##Helper() {                                                 \
            FieldFactory::registerClass(#class_name,                           \
                                        class_name##Helper::creatObjFunc);     \
        }                                                                      \
        static Field *creatObjFunc() { return new class_name; }                \
    };                                                                         \
    class_name##Helper class_name##helper;

#endif