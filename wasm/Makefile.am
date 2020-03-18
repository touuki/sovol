libsovol_wasmlibdir = ${libdir}
libsovol_wasmlib_PROGRAMS = libsovol-wasm.so
libsovol_wasm_so_SOURCES = wasm.cc
libsovol_wasm_so_CXXFLAGS = -I../lib -std=c++17 -Wall --bind
libsovol_wasm_so_LDFLAGS = -shared -fPIC
libsovol_wasm_so_LDADD = ../lib/libsovol.a
