#include "Utils.hh"

#include <ctime>

#include "Config.hh"

thread_local std::mt19937 Utils::e(Config::getInt(SOVOL_CONFIG_KEY(RANDOM_SEED),
                                     time(NULL)));