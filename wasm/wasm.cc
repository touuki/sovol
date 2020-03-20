#include "Config.hh"
#include "RealTimeRepeatSimulation.hh"
#include "Utils.hh"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

RealTimeRepeatSimulation *initSimulation(val params) {
    val keys = val::global("Object").call<val>("keys", params);
    int length = keys["length"].as<int>();
    for (int i = 0; i < length; ++i) {
        Config::setString(keys[i].as<std::string>().c_str(),
                          params[keys[i]].as<std::string>().c_str());
    }
    return new RealTimeRepeatSimulation;
};

val getData(RealTimeRepeatSimulation *simulation) {
    val result = val::object();
    Particle *particle = simulation->getParticle();
    result.set("t", simulation->getCurrentTime());
    result.set("x", particle->position.getX());
    result.set("y", particle->position.getY());
    result.set("z", particle->position.getZ());
    result.set("px", particle->momentum.getX());
    result.set("py", particle->momentum.getY());
    result.set("pz", particle->momentum.getZ());
    result.set("Ek", Utils::kineticEnergy(particle->momentum, particle->mass));
    return result;
};

val runAndGetData(RealTimeRepeatSimulation *simulation) {
    SimulationStatus status = simulation->run(100);
    val result = val::object();
    switch (status) {
    case SimulationStatus::DATA_OUTPUT:
        result.set("data", getData(simulation));
        break;
    case SimulationStatus::ENDTIME_REACHED:
        result.set("isEndTime", true);
        result.set("data", getData(simulation));
        break;
    case SimulationStatus::FINISHED:
        result.set("finished", true);
        break;
    case SimulationStatus::TIME_LIMIT_EXCEED:
        break;
    default:
        break;
    }
    return result;
};

EMSCRIPTEN_BINDINGS(module) {
    function("initSimulation", &initSimulation,
             allow_raw_pointer<RealTimeRepeatSimulation>());
    function("runAndGetData", &runAndGetData,
             allow_raw_pointer<RealTimeRepeatSimulation>());
    class_<RealTimeRepeatSimulation>("RealTimeRepeatSimulation");
}

/*
export EMCC_WASM_BACKEND=1
em++ -o libsovol-wasm.js libsovol-wasm.so --bind -s TOTAL_MEMORY=200MB -s \
ALLOW_TABLE_GROWTH=1
*/