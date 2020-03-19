#include "RealTimeRepeatSimulation.hh"
#include "Utils.hh"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace emscripten;

RealTimeRepeatSimulation *initSimulation(val params){

};

val runAndGetData(RealTimeRepeatSimulation *simulation){
    SimulationStatus status = simulation->run(100);

};

EMSCRIPTEN_BINDINGS(module) {
    function("initSimulation", &initSimulation,
             allow_raw_pointer<RealTimeRepeatSimulation>());
    function("runAndGetData", &runAndGetData,
             allow_raw_pointer<RealTimeRepeatSimulation>());
    class_<RealTimeRepeatSimulation>("RealTimeRepeatSimulation");
}

// export EMCC_WASM_BACKEND=1
// em++ -o libsovol-wasm.js libsovol-wasm.so --bind -s TOTAL_MEMORY=200MB -s
// ALLOW_TABLE_GROWTH=1