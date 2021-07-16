#ifndef _SOVOL_DISTRIBUTION_HH
#define _SOVOL_DISTRIBUTION_HH 1

#include <random>

template <typename T>
class Distribution {
 private:
  int type;
  std::uniform_real_distribution<T> *urd;
  std::normal_distribution<T> *nd;
  void destruct() {
    switch (type) {
      case 0:
        delete urd;
        urd = nullptr;
        break;
      case 1:
        delete nd;
        nd = nullptr;
        break;
      default:
        break;
    }
    type = -1;
  };

 public:
  Distribution(const Distribution &) = delete;
  Distribution &operator=(const Distribution &) = delete;
  Distribution(Distribution &&_d) : type(_d.type), urd(_d.urd), nd(_d.nd) {
    switch (_d.type) {
      case 0:
        _d.urd = nullptr;
      case 1:
        _d.nd = nullptr;
    }
    _d.type = -1;
  };
  Distribution &operator=(Distribution &&_d) {
    destruct();
    type = _d.type;
    switch (_d.type) {
      case 0:
        urd = _d.urd;
        _d.urd = nullptr;
      case 1:
        nd = _d.nd;
        _d.nd = nullptr;
    }
    _d.type = -1;
  };
  Distribution(const std::uniform_real_distribution<T> &_urd)
      : type(0), urd(new std::uniform_real_distribution(_urd)), nd(nullptr){};
  Distribution(const std::normal_distribution<T> &_nd)
      : type(1), urd(nullptr), nd(new std::normal_distribution(_nd)){};
  ~Distribution() { destruct(); };
  template <typename _UniformRandomNumberGenerator>
  T operator()(_UniformRandomNumberGenerator &__urng) {
    switch (type) {
      case 0:
        return (*urd)(__urng);
      case 1:
        return (*nd)(__urng);
    }
    return 0;
  };
};

#endif