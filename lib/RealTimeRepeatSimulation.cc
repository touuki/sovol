#include "RealTimeRepeatSimulation.hh"
#include "Config.hh"
#include "FieldFactory.hh"
#include "Utils.hh"
#include <ctime>

RealTimeRepeatSimulation::RealTimeRepeatSimulation()
    : RealTimeRepeatSimulation(
          FieldFactory::createObject(Config::getString(
              SOVOL_CONFIG_KEY(FIELD_CLASSNAME))),
          new ParticleFactory(),
          Config::getDouble(
              SOVOL_CONFIG_KEY(REALTIMEREPEATSIMULATION_TIMESTEP)),
          Config::getDouble(SOVOL_CONFIG_KEY(REALTIMEREPEATSIMULATION_ENDTIME)),
          Config::getInt(
              SOVOL_CONFIG_KEY(REALTIMEREPEATSIMULATION_REMAININGNUMBER), 1),
          Config::getDouble(
              SOVOL_CONFIG_KEY(REALTIMEREPEATSIMULATION_DATAINTERVAL)),
          Config::getDouble(
              SOVOL_CONFIG_KEY(REALTIMEREPEATSIMULATION_DATASTARTTIME))){};

RealTimeRepeatSimulation::RealTimeRepeatSimulation(
    Field *_field, ParticleFactory *_particleFactory, double _timeStep,
    double _endTime, int _remainingNumber, double _dataInterval,
    double _dataStartTime)
    : field(_field), particleFactory(_particleFactory),
      timeStep(abs(_timeStep)), endTime(abs(_endTime)),
      remainingNumber(_remainingNumber > 1 ? _remainingNumber : 1),
      dataInterval(abs(_dataInterval)),
      dataStartTime(_dataStartTime > endTime ? endTime : _dataStartTime),
      currentParticle(_particleFactory->createObject()), currentTime(0.),
      nextDataTime(0.){};

RealTimeRepeatSimulation::~RealTimeRepeatSimulation() {
    if (field != nullptr)
        delete field;
    if (particleFactory != nullptr)
        delete particleFactory;
    setParticle(nullptr);
};

double RealTimeRepeatSimulation::calculateNextDataTime() {
    if (currentTime < nextDataTime) {
        return nextDataTime;
    }
    if (currentTime < dataStartTime) {
        return dataStartTime;
    }
    if (dataInterval <= 0.) {
        return endTime;
    }
    if (currentTime < endTime && currentTime + dataInterval < endTime) {
        return currentTime + dataInterval;
    }
    return endTime;
};

void RealTimeRepeatSimulation::setParticle(Particle *_particle) {
    if (currentParticle != _particle && currentParticle != nullptr) {
        delete currentParticle;
    }
    currentParticle = _particle;
};

Particle *RealTimeRepeatSimulation::getParticle() const {
    return currentParticle;
};

double RealTimeRepeatSimulation::getCurrentTime() const { return currentTime; };

SimulationStatus RealTimeRepeatSimulation::run(long timeLimitMilliseconds) {
    if (remainingNumber <= 0) {
        // simulation is over
        return SimulationStatus::FINISHED;
    }

    if (currentTime >= endTime) {
        // next Particle
        setParticle(particleFactory->createObject());
        currentTime = 0.;
        nextDataTime = 0.;
    }

    long startClock = clock();
    nextDataTime = calculateNextDataTime();

    for (int i = 1; currentTime < nextDataTime; i++) {
        currentParticle->nextStep(*field, currentTime, timeStep);
        currentTime += timeStep;
        if (i % 1000 == 0 && timeLimitMilliseconds > 0 &&
            1000 / CLOCKS_PER_SEC * (clock() - startClock) >
                timeLimitMilliseconds) {
            break;
        }
    }

    if (currentTime >= endTime) {
        remainingNumber--;
        return SimulationStatus::ENDTIME_REACHED;
    }

    if (currentTime < nextDataTime) {
        return SimulationStatus::TIME_LIMIT_EXCEED;
    }

    return SimulationStatus::DATA_OUTPUT;
};
