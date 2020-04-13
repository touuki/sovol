#ifndef _SOVOL_WASMSIMULATION_HH
#define _SOVOL_WASMSIMULATION_HH 1

#include "RealTimeRepeatSimulation.hh"
#include <emscripten/val.h>

using namespace emscripten;

class WasmSimulation
{
private:
    RealTimeRepeatSimulation *simulation;
    int id;
    bool start;
    void setSimulation(RealTimeRepeatSimulation *);
public:
    WasmSimulation();
    int getId() const;
    bool isStart() const;
    void stop();
    void init(val params);
    virtual ~WasmSimulation();
    virtual val getData();
    val runAndGetData(int _id);
};

#endif