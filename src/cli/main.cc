#include <unistd.h>

#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <thread>

#include "BoundedBuffer.hh"
#include "Simulator.hh"
#include "Storage.hh"

#define TIME_FORMAT_STRING "%Y-%m-%d %H:%M:%S"

int main(int argc, char *const argv[]) {

  double timeStep = Config::getDouble(SOVOL_CONFIG_KEY(SIMULATION_TIMESTEP));
  double dataStartTime = Config::getDouble(SOVOL_CONFIG_KEY(DATA_STARTTIME));
  double dataEndTime = Config::getDouble(SOVOL_CONFIG_KEY(DATA_ENDTIME));
  int dataInterval = Config::getInt(SOVOL_CONFIG_KEY(DATA_INTERVAL), 1);

  // auto dataInterval for 0
  if (dataInterval <= 0) {
    dataInterval = (dataEndTime - dataStartTime) / timeStep;
  }
  hsize_t length = (dataEndTime - dataStartTime) / timeStep / dataInterval + 1;

  std::vector<hsize_t> timePoints(length);
  hsize_t step = dataStartTime / timeStep;
  for (hsize_t i = 0; i < length; i++, step += dataInterval) {
    timePoints[i] = step;
  }

  std::stringstream ofss;
  const char *output_name = Config::getString(SOVOL_CONFIG_KEY(OUTPUT_NAME));
  if (output_name != nullptr) {
    ofss << output_name;
  } else {
    ofss << "result-p" << getpid() << ".h5";
  }

  H5Storage storage(ofss.str(),
                    std::string(Config::getString(SOVOL_CONFIG_KEY(DATA_ITEMS),
                                                  "x,y,z,px,py,pz")),
                    length);

  double *times = new double[length];
  for (hsize_t i = 0; i < length; i++) {
    times[i] = timeStep * timePoints[i];
  }

  storage.writeTime(times);
  delete[] times;

  Simulator simulator(
      FieldFactory::createObject(
          Config::getString(SOVOL_CONFIG_KEY(FIELD_CLASSNAME), "CustomField")),
      AlgorithmFactory::createObject(Config::getString(
          SOVOL_CONFIG_KEY(ALGORITHM_CLASSNAME), "LeapfrogAlgorithm")),
      std::move(timePoints), timeStep);

  int workers = Config::getInt(SOVOL_CONFIG_KEY(PARALLEL_WORKERS), 1);
  BoundedBuffer<std::shared_ptr<Particle>> particleBuffer(2 * workers, 1);
  BoundedBuffer<std::map<std::string, std::vector<double>>> resultBuffer(
      2 * workers, workers);

  std::thread storage_thread([&resultBuffer, &storage] {
    int count = 0;
    time_t last = time(NULL);
    std::cout << "[" << std::put_time(localtime(&last), TIME_FORMAT_STRING)
              << "] Start simulation." << std::endl;
    try {
      while (true) {
        time_t now = time(NULL);
        if (now - last > 60) {
          std::cout << "[" << std::put_time(localtime(&now), TIME_FORMAT_STRING)
                    << "] Finished " << count << " Particles." << std::endl;
          last = now;
        }
        storage.write(resultBuffer.pop());
        count++;
      }
    } catch (const NoProducerException &e) {
    }

    last = time(NULL);
    std::cout << "[" << std::put_time(localtime(&last), TIME_FORMAT_STRING)
              << "] Finish simulation. Total particles is " << count
              << std::endl;
  });

  std::vector<std::thread> simulator_threads;
  for (int i = 0; i < workers; i++) {
    simulator_threads.push_back(std::thread([&particleBuffer, &resultBuffer,
                                             &simulator] {
      Utils::e.seed(std::hash<std::thread::id>{}(std::this_thread::get_id()));
      try {
        while (true) {
          resultBuffer.push(simulator(particleBuffer.pop()));
        }
      } catch (const NoProducerException &e) {
      }
      resultBuffer.producerQuit();
    }));
  }

  int totalNumber = Config::getInt(SOVOL_CONFIG_KEY(PARTICLES_NUMBER), 1);
  std::shared_ptr<ParticleProducer> particleProducer =
      ParticleProducerFactory::createObject(
          Config::getString(SOVOL_CONFIG_KEY(PARTICLEPRODUCER_CLASSNAME),
                            "BeamParticleProducer"));
  for (int i = 0; i < totalNumber; i++) {
    particleBuffer.push(particleProducer->createParticle());
  }
  particleBuffer.producerQuit();

  storage_thread.join();
  for (auto &t : simulator_threads) {
    t.join();
  }

  return 0;
}