#include "Utils.hh"

#include <ctime>

#include "Config.hh"

std::default_random_engine Utils::e(
    Config::getInt(SOVOL_CONFIG_KEY(RANDOM_SEED), time(NULL)));
std::uniform_real_distribution<double> Utils::dist(0., 1.);