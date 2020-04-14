#ifndef _SOVOL_REALTIMEREPEATSIMULATION_HH
#define _SOVOL_REALTIMEREPEATSIMULATION_HH 1

#include "Algorithm.hh"
#include "Field.hh"
#include "ParticleFactory.hh"
#include <vector>

enum SimulationStatus {
    UNSET,
    DATA_OUTPUT,
    MAX_ITERATION_TIMES_REACHED,
    ENDTIME_REACHED,
    FINISHED
};

class RealTimeRepeatSimulation {
  private:
    Field *field;
    ParticleFactory *particleFactory;
    Algorithm *algorithm;
    double timeStep;
    double endTime;
    int remainingNumber;
    double dataInterval;
    double dataStartTime;

    Particle *currentParticle;
    double currentTime;
    double nextDataTime;

    void setParticle(Particle *);
    double calculateNextDataTime();

  public:
    RealTimeRepeatSimulation();
    RealTimeRepeatSimulation(Field *_field, ParticleFactory *_particleFactory,
                             Algorithm *_algorithm, double _timeStep,
                             double _endTime, int _remainingNumber = 1,
                             double _dataInterval = 0.,
                             double _dataStartTime = 0.);
    ~RealTimeRepeatSimulation();
    Particle *getParticle() const;
    double getCurrentTime() const;
    SimulationStatus run(int32_t maxIterationTimes = -1);
};

#endif