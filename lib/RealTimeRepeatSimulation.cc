#include "RealTimeRepeatSimulation.hh"
#include "Config.hh"
#include "Utils.hh"
#include <ctime>

RealTimeRepeatSimulation::RealTimeRepeatSimulation()
    : RealTimeRepeatSimulation(
          FieldFactory::createObject(Config::getString(
              SOVOL_CONFIG_KEY(FIELD_CLASSNAME), "CustomField")),
          ParticleProducerFactory::createObject(
              Config::getString(SOVOL_CONFIG_KEY(PARTICLEPRODUCER_CLASSNAME),
                                "BeamParticleProducer")),
          AlgorithmFactory::createObject(Config::getString(
              SOVOL_CONFIG_KEY(ALGORITHM_CLASSNAME), "LeapfrogAlgorithm")),
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
    std::shared_ptr<Field> _field,
    std::shared_ptr<ParticleProducer> _particleProducer,
    std::shared_ptr<Algorithm> _algorithm, double _timeStep, double _endTime,
    int _remainingNumber, double _dataInterval, double _dataStartTime)
    : field(_field), particleProducer(_particleProducer), algorithm(_algorithm),
      timeStep(abs(_timeStep)), endTime(abs(_endTime)),
      remainingNumber(_remainingNumber > 1 ? _remainingNumber : 1),
      dataInterval(abs(_dataInterval)),
      dataStartTime(_dataStartTime > endTime ? endTime : _dataStartTime),
      currentParticle(_particleProducer->createParticle()), currentTime(0.),
      nextDataTime(0.){};

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

Particle *RealTimeRepeatSimulation::getParticle() const {
    return currentParticle.get();
};

double RealTimeRepeatSimulation::getCurrentTime() const { return currentTime; };

SimulationStatus RealTimeRepeatSimulation::run(int32_t maxIterationTimes) {
    if (remainingNumber <= 0) {
        return SimulationStatus::FINISHED;
    }

    if (currentTime >= endTime) {
        currentParticle = particleProducer->createParticle();
        currentTime = 0.;
        nextDataTime = 0.;
    }

    if (maxIterationTimes < 0) {
        maxIterationTimes = INT32_MAX;
    }

    nextDataTime = calculateNextDataTime();

    for (int32_t i = 0; currentTime < nextDataTime && i < maxIterationTimes;
         i++) {
        (*algorithm)(currentParticle.get(), field.get(), currentTime, timeStep);
        currentTime += timeStep;
    }

    if (currentTime >= endTime) {
        remainingNumber--;
        return SimulationStatus::ENDTIME_REACHED;
    }

    if (currentTime < nextDataTime) {
        return SimulationStatus::MAX_ITERATION_TIMES_REACHED;
    }

    return SimulationStatus::DATA_OUTPUT;
};
