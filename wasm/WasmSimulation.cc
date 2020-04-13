#include "WasmSimulation.hh"
#include "Config.hh"
#include "Utils.hh"
#include "WasmSimulationFactory.hh"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

REGISTER_WASMSIMULATION(WasmSimulation)

WasmSimulation::WasmSimulation() : simulation(nullptr), id(0), start(false){};

void WasmSimulation::setSimulation(RealTimeRepeatSimulation *_simulation) {
    if (simulation != _simulation) {
        if (simulation != nullptr) {
            delete simulation;
        }
        simulation = _simulation;
    }
};

int WasmSimulation::getId() const{
    return id;
};

bool WasmSimulation::isStart() const{
    return start;
};

void WasmSimulation::stop() {
    start = false;
}

void WasmSimulation::init(val params) {
    val keys = val::global("Object").call<val>("keys", params);
    int length = keys["length"].as<int>();
    for (int i = 0; i < length; ++i) {
        Config::setString(keys[i].as<std::string>().c_str(),
                          params[keys[i]].as<std::string>().c_str());
    }
    setSimulation(new RealTimeRepeatSimulation);
    start = true;
    id++;
};

WasmSimulation::~WasmSimulation() { setSimulation(nullptr); }

val WasmSimulation::getData() {
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

val WasmSimulation::runAndGetData(int _id) {
    val result = val::object();
    if (!start) {
        result.set("aborted", true);
        result.set("message", "Stopped.");
        return result;
    }
    if (id != _id) {
        result.set("aborted", true);
        result.set("message", "Inconsistent ID.");
        return result;
    }
    SimulationStatus status = simulation->run();
    switch (status) {
    case SimulationStatus::DATA_OUTPUT:
        result.set("data", getData());
        break;
    case SimulationStatus::ENDTIME_REACHED:
        result.set("isEndTime", true);
        result.set("data", getData());
        break;
    case SimulationStatus::FINISHED:
        result.set("finished", true);
        start = false;
        break;
    case SimulationStatus::MAX_ITERATION_TIMES_REACHED:
        break;
    default:
        break;
    }
    return result;
};

EMSCRIPTEN_BINDINGS(module) {
    function("getWasmSimulation", &WasmSimulationFactory::getWasmSimulation,
             allow_raw_pointer<WasmSimulation>());
    class_<WasmSimulation>("WasmSimulation")
        .function("getId", &WasmSimulation::getId)
        .function("isStart", &WasmSimulation::isStart)
        .function("stop", &WasmSimulation::stop)
        .function("init", &WasmSimulation::init)
        .function("runAndGetData", &WasmSimulation::runAndGetData);
}