include_HEADERS = \
    Algorithm.hh \
    AlgorithmFactory.hh \
    Config.hh \
    Field.hh \
    FieldFactory.hh \
    LaguerreGaussianPulseField.hh \
    Particle.hh \
    ParticleFactory.hh \
    ParticleFactoryProducer.hh \
    RealTimeRepeatSimulation.hh \
    Utils.hh \
    Vector3.hh
if WASM
libsovollibdir = ${libdir}
libsovollib_PROGRAMS = libsovol.so
libsovol_so_SOURCES = \
    Algorithm.cc \
    Field.cc \
    LaguerreGaussianPulseField.cc \
    Particle.cc \
    ParticleFactory.cc \
    RealTimeRepeatSimulation.cc \
    Utils.cc 
libsovol_so_CXXFLAGS = -std=c++17 -Wall
libsovol_so_LDFLAGS = -shared -fPIC
else
lib_LIBRARIES = libsovol.a
libsovol_a_SOURCES = \
    Algorithm.cc \
    Field.cc \
    LaguerreGaussianPulseField.cc \
    Particle.cc \
    ParticleFactory.cc \
    RealTimeRepeatSimulation.cc \
    Utils.cc 
libsovol_a_CXXFLAGS = -std=c++17 -Wall
endif