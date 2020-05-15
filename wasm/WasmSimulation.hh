#ifndef _SOVOL_WASMSIMULATION_HH
#define _SOVOL_WASMSIMULATION_HH 1

#include "FactoryHelper.hh"
#include "RealTimeRepeatSimulation.hh"
#include <emscripten/val.h>

class WasmSimulation {
  protected:
    std::shared_ptr<RealTimeRepeatSimulation> simulation;
    int id;
    bool start;
    SimulationStatus lastStatus;
    emscripten::val storedData;
    virtual void storeData(bool isNewParticle);
    virtual emscripten::val getData() const;

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