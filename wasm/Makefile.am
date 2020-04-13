libsovol_wasmlibdir = ${libdir}
libsovol_wasmlib_PROGRAMS = libsovol-wasm.so
include_HEADERS = WasmSimulation.hh WasmSimulationFactory.hh
libsovol_wasm_so_SOURCES = WasmSimulation.cc
libsovol_wasm_so_CXXFLAGS = -I../lib -std=c++17 -Wall --bind
libsovol_wasm_so_LDFLAGS = -shared -fPIC