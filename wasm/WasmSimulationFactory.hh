#ifndef _SOVOL_WASMSIMULATIONFACTORY_HH
#define _SOVOL_WASMSIMULATIONFACTORY_HH 1

#include "WasmSimulation.hh"
#include <map>
#include <string>

typedef WasmSimulation *(*WasmSimulationConstructor)();

class WasmSimulationFactory {
  public:
    static void registerClass(const std::string &className,
                              WasmSimulationConstructor constructor) {
        constructors()[className] = constructor;
    }

    static WasmSimulation *getWasmSimulation(const std::string &className) {
        if (instances().find(className) != instances().end())
            return instances().find(className)->second;

        WasmSimulationConstructor constructor = nullptr;

        if (constructors().find(className) != constructors().end())
            constructor = constructors().find(className)->second;

        if (constructor == nullptr) {
            std::cerr << "ERROR: Create WasmSimulation failed. Class " << className
                      << " is not found." << std::endl;
            return nullptr;
        }

        WasmSimulation *object = (*constructor)();
        instances()[className] = object;
        return object;
    }

  private:
    inline static std::map<std::string, WasmSimulationConstructor> &constructors() {
        static std::map<std::string, WasmSimulationConstructor> instance;
        return instance;
    }

    inline static std::map<std::string, WasmSimulation *> &instances() {
        static std::map<std::string, WasmSimulation *> instance;
        return instance;
    }
};

#define REGISTER_WASMSIMULATION(class_name)                                    \
    class class_name##Helper {                                                 \
      public:                                                                  \
        class_name##Helper() {                                                 \
            WasmSimulationFactory::registerClass(                              \
                #class_name, class_name##Helper::creatObjFunc);                \
        }                                                                      \
        static WasmSimulation *creatObjFunc() { return new class_name; }       \
    };                                                                         \
    class_name##Helper class_name##helper;

#endif