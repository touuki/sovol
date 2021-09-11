#include "Field.hh"

#include <complex>
#include <vector>

#include "FieldShifter.hh"
#include "utils.hh"

class CustomField : public Field {
#ifdef __EMSCRIPTEN__
 private:
  emscripten::val func;

 public:
  CustomField(emscripten::val v) : func(v["func"]){};
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    return EMField<double>(func(pos.x, pos.y, pos.z, t));
  };
  ~CustomField(){};
#else
 private:
  std::string global_function_name;

 public:
  CustomField(Lua &lua)
      : global_function_name(
            lua.getField<std::string>("global_function_name")){};

  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    Lua &lua = Lua::getInstance();
    lua.visitGlobal(global_function_name.c_str());
    lua.call(1, pos.x, pos.y, pos.z, t);
    EMField<double> em(lua);
    lua.leaveTable();
    return em;
  };
#endif
};

REGISTER_MULTITON(Field, CustomField)

class FieldWithShifters : public Field {
 private:
  std::shared_ptr<Field> field;
  std::vector<std::shared_ptr<FieldShifter>> shifters;

 public:
  FieldWithShifters(const std::shared_ptr<Field> &_field,
                    const std::vector<std::shared_ptr<FieldShifter>> &_shifters)
      : field(_field), shifters(_shifters){};
#ifdef __EMSCRIPTEN__
  FieldWithShifters(emscripten::val v)
      : field(Field::createObject(v["field"])) {
    emscripten::val val_shifters = v["shifters"];
    if (!val_shifters.isUndefined()) {
      for (int i = 0; i < val_shifters["length"].as<int>(); i++) {
        shifters.push_back(FieldShifter::createObject(val_shifters[i]));
      }
    }
  };
#else
  FieldWithShifters(Lua &lua)
      : field(lua.getField<std::shared_ptr<Field>>("field")),
        shifters(lua.getField<std::vector<std::shared_ptr<FieldShifter>>>(
            "shifters")){};
#endif
  EMField<double> operator()(const Vector3<double> &_pos,
                             double t) const override {
    Vector3<double> pos(_pos);
    for (auto it = shifters.rbegin(); it != shifters.rend(); ++it) {
      pos = (*it)->reversePosition(pos);
    }
    EMField<double> em(field->operator()(pos, t));
    for (auto &&shifter : shifters) {
      em = shifter->shiftEMField(em);
    }
    return em;
  };
};

REGISTER_MULTITON(Field, FieldWithShifters)

class FieldCombiner : public Field {
 private:
  std::vector<std::shared_ptr<Field>> fields;

 public:
  FieldCombiner(const std::vector<std::shared_ptr<Field>> &_fields)
      : fields(_fields){};
#ifdef __EMSCRIPTEN__
  FieldCombiner(emscripten::val v) {
    emscripten::val val_fields = v["fields"];
    if (!val_fields.isUndefined()) {
      for (int i = 0; i < val_fields["length"].as<int>(); i++) {
        fields.push_back(Field::createObject(val_fields[i]));
      }
    }
  };
#else
  FieldCombiner(Lua &lua)
      : fields(lua.getField<std::vector<std::shared_ptr<Field>>>("fields")){};
#endif
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    EMField<double> em;
    for (auto &&field : fields) {
      em += field->operator()(pos, t);
    }
    return em;
  };
};

REGISTER_MULTITON(Field, FieldCombiner)

class LaguerreGaussianPulseField : public Field {
 private:
  //这个a0理解为能量归一化参数
  //它使得相同w0和tau不同模式的激光归一化到相同的能量
  //只有对于基态模式它才代表腰处的矢势强度
  double a0;
  double tau;
  double w0;
  double zR;
  double ex;
  double ey;
  double delay;
  int p;
  int l;
  double iphase;
  double k;  // 2 * PI / lambda
  double h;  // unit is lambda when input

  double amplitude(double x, double y, double z) const {
    double r2 = std::pow(x, 2) + std::pow(y, 2);
    double wz = w0 * std::sqrt(1. + std::pow(z / zR, 2));
    double item1 = l == 0 ? 1. : std::pow(std::sqrt(2. * r2) / wz, std::abs(l));
    double item2 = p == 0 ? 1.
                          : utils::generLaguePoly(std::abs(l), p,
                                                  2. * r2 / std::pow(wz, 2));
    return a0 * (w0 / wz) * item1 * item2 * std::exp(-r2 / std::pow(wz, 2));
  };

  double envelope(double x, double y, double z, double t) const {
    double r2 = std::pow(x, 2) + std::pow(y, 2);
    double irz = z / (std::pow(z, 2) + std::pow(zR, 2));
    double phase = (t - delay) - (z + .5 * r2 * irz);
    return 1. / std::cosh(std::log(3. + 2. * std::sqrt(2)) * phase / tau);
  };

 public:
  LaguerreGaussianPulseField(double _a0, double _tau, double _w0,
                             double _polar = 0., double _delay = 0., int _p = 0,
                             int _l = 0, double _iphase = 0., double _k = 1.,
                             double _h = 0.1)
      : a0(std::fabs(_a0) * std::sqrt((double)utils::factorial(_p) /
                                      utils::factorial(_p + std::abs(_l)))),
        tau(std::fabs(_tau)),
        w0(std::fabs(_w0)),
        zR(0.5 * std::pow(w0, 2)),
        ex(std::sqrt(1. - std::pow(_polar, 2))),
        ey(_polar),
        delay(_delay),
        p(std::abs(_p)),
        l(_l),
        iphase(_iphase),
        k(std::fabs(_k)),
        h(std::fabs(_h) * 2. * M_PI / k){};
#ifdef __EMSCRIPTEN__
  LaguerreGaussianPulseField(emscripten::val v)
      : LaguerreGaussianPulseField(
            v["a0"].as<double>(), v["tau"].as<double>(), v["w0"].as<double>(),
            v["polar"].isUndefined() ? 0. : v["polar"].as<double>(),
            v["delay"].isUndefined() ? 0. : v["delay"].as<double>(),
            v["p"].isUndefined() ? 0 : v["p"].as<int>(),
            v["l"].isUndefined() ? 0 : v["l"].as<int>(),
            v["iphase"].isUndefined() ? 0. : v["iphase"].as<double>(),
            v["k"].isUndefined() ? 1. : v["k"].as<double>(),
            v["h"].isUndefined() ? 0.1 : v["h"].as<double>()){};
#else
  LaguerreGaussianPulseField(Lua &lua)
      : LaguerreGaussianPulseField(
            lua.getField<double>("a0"), lua.getField<double>("tau"),
            lua.getField<double>("w0"), lua.getField<double>("polar", 0.),
            lua.getField<double>("delay", 0.), lua.getField<int>("p", 0),
            lua.getField<int>("l", 0), lua.getField<double>("iphase", 0.),
            lua.getField<double>("k", 1.), lua.getField<double>("h", 0.1)){};
#endif
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    using namespace std;
    double x = pos.x;
    double y = pos.y;
    double z = pos.z;
    double r2 = std::pow(x, 2) + std::pow(y, 2);
    double irz = z / (std::pow(z, 2) + std::pow(zR, 2));
    double psiz = (std::abs(l) + 2 * p + 1) * std::atan(z / zR);
    double lphi = l == 0 || r2 == 0 ? 0. : l * std::atan2(y, x);
    double u0 = amplitude(x, y, z);
    complex<double> common =
        std::exp(1.i * ((t - z - .5 * r2 * irz) * k + iphase - lphi + psiz)) *
        envelope(x, y, z, t);

    complex<double> pupx, pupy;
    if (p == 0) {
      complex<double> factor = k * u0 / (1.i * z - zR);
      if (l == 0) {
        pupx = factor * x;
        pupy = factor * y;
      } else {
        if (r2 == 0) {
          if (std::abs(l) == 1) {
            double fact = a0 * std::sqrt(2.) / (w0 * (1 + std::pow(z / zR, 2)));
            pupx = fact;
            pupy = -1.i * fact;
          } else {
            pupx = 0;
            pupy = 0;
          }
        } else {
          pupx = factor * x + u0 * (x + 1.i * static_cast<double>(l) * y) / r2;
          pupy = factor * y + u0 * (y - 1.i * static_cast<double>(l) * x) / r2;
        }
      }
    } else {
      pupx = (amplitude((x + h) * x < 0 ? x * 0.5 : x + h, y, z) -
              amplitude((x - h) * x < 0 ? x * 0.5 : x - h, y, z)) /
                 (2. * h) -
             1.i * u0 * (k * x * irz - l * y / r2);
      pupy = (amplitude(x, (y + h) * y < 0 ? y * 0.5 : y + h, z) -
              amplitude(x, (y - h) * y < 0 ? y * 0.5 : y - h, z)) /
                 (2. * h) -
             1.i * u0 * (k * y * irz + l * x / r2);
    }

    complex<double> Ex = -1.i * ex * u0 * common;
    complex<double> Ey = ey * u0 * common;
    complex<double> Ez = -(ex * pupx + ey * pupy * 1.i) * common;
    complex<double> Bx = -ey * u0 * common;
    complex<double> By = -1.i * ex * u0 * common;
    complex<double> Bz = (ey * pupx * 1.i - ex * pupy) * common;
    return EMField<double>{Vector3(Ex.real(), Ey.real(), Ez.real()),
                           Vector3(Bx.real(), By.real(), Bz.real())};
  };
};

REGISTER_MULTITON(Field, LaguerreGaussianPulseField)
