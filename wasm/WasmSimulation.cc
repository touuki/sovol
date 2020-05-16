#include "WasmSimulation.hh"
#include "Config.hh"
#include "Utils.hh"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

REGISTER_SINGLETON(WasmSimulation, WasmSimulation)

WasmSimulation::WasmSimulation()
    : simulation(nullptr), id(0), start(false), lastStatus(UNSET),
      storedData(val::array()){};

WasmSimulation::~WasmSimulation(){};

int WasmSimulation::getId() const { return id; };

bool WasmSimulation::isStart() const { return start; };

void WasmSimulation::stop() { start = false; }

void WasmSimulation::init(val params) {
    val keys = val::global("Object").call<val>("keys", params);
    int length = keys["length"].as<int>();
    for (int i = 0; i < length; ++i) {
        Config::setString(keys[i].as<std::string>().c_str(),
                          params[keys[i]].as<std::string>().c_str());
    }
    simulation = std::make_shared<RealTimeRepeatSimulation>();
    start = true;
    id++;
    lastStatus = UNSET;
    storedData = val::object();
    storedData.set("particles", val::array());
};

val WasmSimulation::getStoredData() const { return storedData; };

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
    simulation_status status = simulation->run();
    bool isNewParticle = lastStatus == ENDTIME_REACHED || lastStatus == UNSET;
    switch (status) {
    case DATA_OUTPUT:
    case ENDTIME_REACHED:
        result.set("data", getData());
        result.set("isNewParticle", isNewParticle);
        storeData(isNewParticle);
        break;
    case FINISHED:
        result.set("finished", true);
        start = false;
        break;
    case MAX_ITERATION_TIMES_REACHED:
        break;
    default:
        break;
    }
    lastStatus = status;
    return result;
};

val WasmSimulation::getData() const {
    val result = val::object();
    std::shared_ptr<Particle> particle = simulation->getParticle();
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

void WasmSimulation::storeData(bool isNewParticle) {
    val particles = storedData["particles"];
    if (isNewParticle) {
        val obj = val::object();
        obj.set("x", val::array());
        obj.set("y", val::array());
        obj.set("z", val::array());
        obj.set("px", val::array());
        obj.set("py", val::array());
        obj.set("pz", val::array());
        obj.set("t", val::array());
        particles.call<val>("push", obj);
    }
    int length = particles["length"].as<int>();
    val current = particles[length - 1];
    std::shared_ptr<Particle> particle = simulation->getParticle();
    current["x"].call<double>("push", particle->position.getX());
    current["y"].call<double>("push", particle->position.getY());
    current["z"].call<double>("push", particle->position.getZ());
    current["px"].call<double>("push", particle->momentum.getX());
    current["py"].call<double>("push", particle->momentum.getY());
    current["pz"].call<double>("push", particle->momentum.getZ());
    current["t"].call<double>("push", simulation->getCurrentTime());
};

EMSCRIPTEN_BINDINGS(module) {
    function("getWasmSimulation", &WasmSimulationFactory::createObject);
    class_<WasmSimulation>("WasmSimulation")
        .smart_ptr_constructor("WasmSimulation",
                               &std::make_shared<WasmSimulation>)
        .function("getId", &WasmSimulation::getId)
        .function("isStart", &WasmSimulation::isStart)
        .function("stop", &WasmSimulation::stop)
        .function("init", &WasmSimulation::init)
        .function("getStoredData", &WasmSimulation::getStoredData)
        .function("runAndGetData", &WasmSimulation::runAndGetData);
}