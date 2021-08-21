#include "WasmSimulation.hh"

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <string>

#include "Utils.hh"

using namespace emscripten;

REGISTER_SINGLETON(WasmSimulation, WasmSimulation)

WasmSimulation::WasmSimulation()
    : simulation(nullptr),
      id(0),
      start(false),
      lastStatus(UNSET),
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
  storedData.set("inputs", params);
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
    case ENDTIME_REACHED:
      result.set("endtimeReached", true);
    case DATA_OUTPUT:
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
  Particle *particle = simulation->getParticle();
  storeScalar(result, "t", simulation->getCurrentTime());
  storeVector(result, "pos", particle->position);
  storeVector(result, "mom", particle->momentum);
  storeVector(result, "E", particle->em.E);
  storeVector(result, "B", particle->em.B);
  storeScalar(result, "Ek",
              Utils::kineticEnergy(particle->momentum, particle->mass));
  return result;
};

void WasmSimulation::storeData(bool isNewParticle) {
  val particles = storedData["particles"];
  if (isNewParticle) {
    val obj = val::object();
    particles.call<val>("push", obj);
  }
  int length = particles["length"].as<int>();
  val current = particles[length - 1];
  Particle *particle = simulation->getParticle();
  storeVector(current, "pos", particle->position, true);
  storeVector(current, "mom", particle->momentum, true);
  storeVector(current, "E", particle->em.E, true);
  storeVector(current, "B", particle->em.B, true);
  storeScalar(current, "t", simulation->getCurrentTime(), true);
  storeScalar(current, "Ek",
              Utils::kineticEnergy(particle->momentum, particle->mass), true);
};

void WasmSimulation::storeVector(val r, const char *s, Vector3<double> &v,
                                 bool asArray) {
  if (asArray) {
    if (r.hasOwnProperty(s)) {
      val o = r[s];
      o["x"].call<double>("push", v.x);
      o["y"].call<double>("push", v.y);
      o["z"].call<double>("push", v.z);
    } else {
      val o = val::object();
      o.set("x", val::array());
      o.set("y", val::array());
      o.set("z", val::array());
      o["x"].call<double>("push", v.x);
      o["y"].call<double>("push", v.y);
      o["z"].call<double>("push", v.z);
      r.set(s, o);
    }
  } else {
    val o = val::object();
    o.set("x", v.x);
    o.set("y", v.y);
    o.set("z", v.z);
    r.set(s, o);
  }
}

void WasmSimulation::storeScalar(val r, const char *s, double v, bool asArray) {
  if (asArray) {
    if (r.hasOwnProperty(s)) {
      r[s].call<double>("push", v);
    } else {
      val a = val::array();
      a.call<double>("push", v);
      r.set(s, a);
    }
  } else {
    r.set(s, v);
  }
}

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