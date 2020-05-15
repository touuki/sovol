#ifndef _SOVOL_REALTIMEREPEATSIMULATION_HH
#define _SOVOL_REALTIMEREPEATSIMULATION_HH 1

#include "Algorithm.hh"
#include "Field.hh"
#include "ParticleProducer.hh"
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
    std::shared_ptr<Field> field;
    std::shared_ptr<ParticleProducer> particleProducer;
    std::shared_ptr<Algorithm> algorithm;
    double timeStep;
    double endTime;
    int remainingNumber;
    double dataInterval;
    double dataStartTime;

    std::shared_ptr<Particle> currentParticle;
    double currentTime;
    double nextDataTime;

    double calculateNextDataTime() const;

  public:
    RealTimeRepeatSimulation();
    RealTimeRepeatSimulation(
        std::shared_ptr<Field> _field,
        std::shared_ptr<ParticleProducer> _particleProducer,
        std::shared_ptr<Algorithm> _algorithm, double _timeStep,
        double _endTime, int _remainingNumber = 1, double _dataInterval = 0.,
        double _dataStartTime = 0.);
    std::shared_ptr<Particle> getParticle() const;
    double getCurrentTime() const;
    std::shared_ptr<Algorithm> getAlgorithm() const;
    SimulationStatus run(int32_t maxIterationTimes = -1);
};

#endif