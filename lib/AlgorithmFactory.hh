#ifndef _SOVOL_ALGORITHMFACTORY_HH
#define _SOVOL_ALGORITHMFACTORY_HH 1

#include "Algorithm.hh"
#include <map>
#include <string>

typedef Algorithm *(*AlgorithmConstructor)();

class AlgorithmFactory {
  public:
    static void registerClass(const std::string &className,
                              AlgorithmConstructor constructor) {
        constructors()[className] = constructor;
    }

    static Algorithm *getAlgorithm(const std::string &className) {
        if (instances().find(className) != instances().end())
            return instances().find(className)->second;

        AlgorithmConstructor constructor = nullptr;

        if (constructors().find(className) != constructors().end())
            constructor = constructors().find(className)->second;

        if (constructor == nullptr) {
            std::cerr << "ERROR: Create Algorithm failed. Class " << className
                      << " is not found." << std::endl;
            return nullptr;
        }

        Algorithm *algorithm = (*constructor)();
        instances()[className] = algorithm;
        return algorithm;
    }

  private:
    inline static std::map<std::string, AlgorithmConstructor> &constructors() {
        static std::map<std::string, AlgorithmConstructor> instance;
        return instance;
    }

    inline static std::map<std::string, Algorithm *> &instances() {
        static std::map<std::string, Algorithm *> instance;
        return instance;
    }
};

#define REGISTER_ALGORITHM(class_name)                                         \
    class class_name##Helper {                                                 \
      public:                                                                  \
        class_name##Helper() {                                                 \
            AlgorithmFactory::registerClass(#class_name,                       \
                                            class_name##Helper::creatObjFunc); \
        }                                                                      \
        static Algorithm *creatObjFunc() { return new class_name; }            \
    };                                                                         \
    class_name##Helper class_name##helper;

#endif