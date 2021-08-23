AUTOMAKE_OPTIONS = subdir-objects
include_HEADERS = \
    src/Algorithm.hh \
    src/config.hh \
    src/Field.hh \
    src/FieldShifter.hh \
    src/Lua.hh \
    src/LuaFactoryHelper.hh \
    src/Particle.hh \
    src/ParticleShifter.hh \
    src/Rotator.hh \
    src/Utils.hh \
    src/Vector3.hh \
    src/WasmFactoryHelper.hh \
    src/wasm/WasmSimulation.hh
if WASM
libsovol_wasmlibdir = $(libdir)
libsovol_wasmlib_PROGRAMS = libsovol-wasm.so
libsovol_wasm_so_SOURCES = \
    src/Algorithm.cc \
    src/Field.cc \
    src/FieldShifter.cc \
    src/Particle.cc \
    src/ParticleShifter.cc \
    src/RealTimeRepeatSimulation.cc \
    src/WasmSimulation.cc
libsovol_wasm_so_CXXFLAGS = -std=c++17 -Wall --bind
libsovol_wasm_so_LDFLAGS = -shared -fPIC
else
lib_LIBRARIES = libsovol.a
libsovol_a_SOURCES = \
    src/cli/Lua.cc \
    src/Algorithm.cc \
    src/Field.cc \
    src/FieldShifter.cc \
    src/Particle.cc \
    src/ParticleShifter.cc \
    src/Utils.cc
libsovol_a_CXXFLAGS = -std=c++17 -Wall
noinst_PROGRAMS = sovol-cli
sovol_cli_SOURCES = \
    src/cli/Lua.cc \
    src/cli/main.cc \
    src/Algorithm.cc \
    src/Field.cc \
    src/FieldShifter.cc \
    src/Particle.cc \
    src/ParticleShifter.cc \
    src/Utils.cc
sovol_cli_CXXFLAGS = -std=c++17 -Wall $(HDF5_CPPFLAGS)
sovol_cli_LDFLAGS = $(HDF5_LDFLAGS)
sovol_cli_LDADD = -llua -ldl -lpthread -lhdf5_cpp $(HDF5_LIBS)
endif