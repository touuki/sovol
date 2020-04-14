#ifndef _SOVOL_WASMSIMULATION_HH
#define _SOVOL_WASMSIMULATION_HH 1

#include "RealTimeRepeatSimulation.hh"
#include "FactoryHelper.hh"
#include <emscripten/val.h>

using namespace emscripten;

class WasmSimulation
{
private:
    RealTimeRepeatSimulation *simulation;
    int id;
    bool start;
    SimulationStatus lastStatus;
    val storedData;
    void setSimulation(RealTimeRepeatSimulation *);
public:
    WasmSimulation();
    int getId() const;
    bool isStart() const;
    void stop();
    void init(val params);
    val getStoredData() const;
    val runAndGetData(int _id);
    virtual ~WasmSimulation();
    virtual void storeData(bool isNewParticle);
    virtual val getData() const;
};

DEFINE_FACTORY(WasmSimulation)

#endif