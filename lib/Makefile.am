include_HEADERS = \
    Algorithm.hh \
    config.hh \
    Field.hh \
    FieldShifter.hh \
    Lua.hh \
    LuaFactoryHelper.hh \
    Particle.hh \
    ParticleShifter.hh \
    RealTimeRepeatSimulation.hh \
    Rotator.hh \
    Utils.hh \
    Vector3.hh \
    WasmFactoryHelper.hh
if WASM
libsovollibdir = $(libdir)
libsovollib_PROGRAMS = libsovol.so
libsovol_so_SOURCES = \
    Algorithm.cc \
    Field.cc \
    FieldShifter.cc \
    Particle.cc \
    ParticleShifter.cc \
    RealTimeRepeatSimulation.cc
libsovol_so_CXXFLAGS = -std=c++17 -Wall
libsovol_so_LDFLAGS = -shared -fPIC
else
lib_LIBRARIES = libsovol.a
libsovol_a_SOURCES = \
    Algorithm.cc \
    Field.cc \
    FieldShifter.cc \
    Lua.cc \
    Particle.cc \
    ParticleShifter.cc \
    RealTimeRepeatSimulation.cc
libsovol_a_CXXFLAGS = -std=c++17 -Wall
endif