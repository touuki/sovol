#ifndef _SOVOL_CONFIG_HH
#define _SOVOL_CONFIG_HH 1

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define SOVOL_CONFIG_KEY(key) "SC_" #key

class Config {
 public:
  static bool getBool(const char *key, bool defaultValue = false) {
    const char *value;
    if ((value = getenv(key)) != nullptr) {
      if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 ||
          strcmp(value, "True") == 0 || strcmp(value, "TRUE") == 0) {
        return true;
      } else {
        return false;
      }
    } else {
      return defaultValue;
    }
  };
  static int setBool(const char *key, bool value) {
    return setenv(key, value ? "true" : "false", 1);
  };
  static double getDouble(const char *key, double defaultValue = 0.) {
    const char *value;
    if ((value = getenv(key)) != nullptr) {
      return atof(value);
    } else {
      return defaultValue;
    }
  };
  static int setDouble(const char *key, double value) {
    char s[64];
    sprintf(s, "%.16g", value);
    return setenv(key, s, 1);
  };
  static int getInt(const char *key, int defaultValue = 0) {
    const char *value;
    if ((value = getenv(key)) != nullptr) {
      return atoi(value);
    } else {
      return defaultValue;
    }
  };
  static int setInt(const char *key, int value) {
    char s[64];
    sprintf(s, "%d", value);
    return setenv(key, s, 1);
  };
  template <typename T>
  static T getPointer(const char *key, T defaultValue = nullptr) {
    const char *value;
    if ((value = getenv(key)) != nullptr) {
      return reinterpret_cast<T>(atol(value));
    } else {
      return defaultValue;
    }
  };
  template <typename T>
  static int setPointer(const char *key, T value) {
    char s[64];
    sprintf(s, "%ld", reinterpret_cast<long>(value));
    return setenv(key, s, 1);
  };
  static const char *getString(const char *key,
                               const char *defaultValue = nullptr) {
    const char *value;
    if ((value = getenv(key)) != nullptr) {
      return value;
    } else {
      return defaultValue;
    }
  };
  static int setString(const char *key, const char *value) {
    return setenv(key, value, 1);
  };
  static bool hasKey(const char *key) { return getenv(key) != nullptr; };
};

#endif