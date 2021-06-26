#ifndef _SOVOL_WASMSIMULATION_HH
#define _SOVOL_WASMSIMULATION_HH 1

#include <emscripten/val.h>

#include "FactoryHelper.hh"
#include "RealTimeRepeatSimulation.hh"

class WasmSimulation {
 protected:
  std::shared_ptr<RealTimeRepeatSimulation> simulation;
  int id;
  bool start;
  simulation_status lastStatus;
  emscripten::val storedData;
  virtual void storeData(bool isNewParticle);
  virtual emscripten::val getData() const;
  static void storeVector(emscripten::val object, const char *key,
                          Vector3<double> &vector, bool asArray = false);
  static void storeScalar(emscripten::val object, const char *key,
                          double scalar, bool asArray = false);

 public:
  WasmSimulation();
  int getId() const;
  bool isStart() const;
  void stop();
  virtual void init(emscripten::val params);
  emscripten::val getStoredData() const;
  emscripten::val runAndGetData(int _id);
  virtual ~WasmSimulation();
};

DEFINE_FACTORY(WasmSimulation)

#endif