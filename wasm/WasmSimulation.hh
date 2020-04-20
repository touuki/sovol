#ifndef _SOVOL_WASMSIMULATION_HH
#define _SOVOL_WASMSIMULATION_HH 1

#include "FactoryHelper.hh"
#include "RealTimeRepeatSimulation.hh"
#include <emscripten/val.h>

using namespace emscripten;

class WasmSimulation {
  protected:
    std::shared_ptr<RealTimeRepeatSimulation> simulation;
    int id;
    bool start;
    SimulationStatus lastStatus;
    val storedData;
    virtual void storeData(bool isNewParticle);
    virtual val getData() const;

  public:
    WasmSimulation();
    int getId() const;
    bool isStart() const;
    void stop();
    void init(val params);
    val getStoredData() const;
    val runAndGetData(int _id);
    virtual ~WasmSimulation();
};

DEFINE_FACTORY(WasmSimulation)

#endif