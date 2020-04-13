#ifndef _SOVOL_PARTICLEFACTORYPRODUCER_HH
#define _SOVOL_PARTICLEFACTORYPRODUCER_HH 1

#include "ParticleFactory.hh"
#include <map>
#include <string>

typedef ParticleFactory *(*ParticleFactoryConstructor)();

class ParticleFactoryProducer {
  public:
    static void registerClass(const std::string &className,
                              ParticleFactoryConstructor constructor) {
        constructors()[className] = constructor;
    }

    static ParticleFactory *createFactory(const std::string &className) {
        ParticleFactoryConstructor constructor = nullptr;

        if (constructors().find(className) != constructors().end())
            constructor = constructors().find(className)->second;

        if (constructor == nullptr) {
            std::cerr << "ERROR: Create ParticleFactory failed. Class "
                      << className << " is not found." << std::endl;
            return nullptr;
        }

        return (*constructor)();
    }

  private:
    inline static std::map<std::string, ParticleFactoryConstructor> &
    constructors() {
        static std::map<std::string, ParticleFactoryConstructor> instance;
        return instance;
    }
};

#define REGISTER_PARTICLEFACTORY(class_name)                                   \
    class class_name##Helper {                                                 \
      public:                                                                  \
        class_name##Helper() {                                                 \
            ParticleFactoryProducer::registerClass(                            \
                #class_name, class_name##Helper::creatObjFunc);                \
        }                                                                      \
        static ParticleFactory *creatObjFunc() { return new class_name; }      \
    };                                                                         \
    class_name##Helper class_name##helper;

#endif