#include "Table.hh"

#include <cmath>
#include <cstring>
#include <iostream>

#ifndef ERROR
#define ERROR(__txt) std::cerr << "ERROR:" << __txt << std::endl;
#endif
#ifndef WARNING
#define WARNING(__txt) std::cerr << "WARNING:" << __txt << std::endl;
#endif

enum table_method {
  TABLE_LINEAR_METHOD,
  TABLE_LOG_METHOD,
  TABLE_EXP_METHOD,
  TABLE_POWER_METHOD
};

double inter_or_extra_polate(double x, double x1, double x2, double y1,
                             double y2, table_method method) {
  if (method == TABLE_LOG_METHOD || method == TABLE_POWER_METHOD) {
    x = std::log10(x);
    x1 = std::log10(x1);
    x2 = std::log10(x2);
  }
  if (method == TABLE_EXP_METHOD || method == TABLE_POWER_METHOD) {
    y1 = std::log10(y1);
    y2 = std::log10(y2);
  }

  double fx = (x - x1) / (x2 - x1);
  double y = (1. - fx) * y1 + fx * y2;

  if (method == TABLE_EXP_METHOD || method == TABLE_POWER_METHOD) {
    y = std::pow(10., y);
  }

  return y;
};

double table_extrapolate(double x, double x1, double x2, double y1, double y2,
                         table_extrapolation_method method) {
  switch (method) {
    case TABLE_CONSTANT_EXTRA:
      if (x < x1) {
        return y1;
      } else if (x > x2) {
        return y2;
      } else {
        ERROR("The value is inside the table.")
        return 0.;
      }
    case TABLE_LINEAR_EXTRA:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_LINEAR_METHOD);
    case TABLE_LOG_EXTRA:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_LOG_METHOD);
    case TABLE_EXP_EXTRA:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_EXP_METHOD);
    case TABLE_POWER_EXTRA:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_POWER_METHOD);
    case TABLE_NO_EXTRA:
    default:
      ERROR("Table extrapolation is disabled.")
      return 0.;
  }
};

double table_interpolate(double x, double x1, double x2, double y1, double y2,
                         table_interpolation_method method) {
  switch (method) {
    case TABLE_LOG_INTER:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_LOG_METHOD);
      break;
    case TABLE_EXP_INTER:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_EXP_METHOD);
      break;
    case TABLE_POWER_INTER:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_POWER_METHOD);
      break;
    case TABLE_LINEAR_INTER:
    default:
      return inter_or_extra_polate(x, x1, x2, y1, y2, TABLE_LINEAR_METHOD);
      break;
  }
};

double table_search(double x, double *xarr, double *yarr, int32_t len,
                    bool arith_seq, table_interpolation_method in_meth,
                    table_extrapolation_method lt_meth,
                    table_extrapolation_method gt_meth) {
  int32_t i1 = 0;
  int32_t i2 = len - 1;
  double xdif1 = xarr[i1] - x;
  double xdif2 = xarr[i2] - x;
  if (xdif1 * xdif2 < 0.) {
    if (arith_seq) {
      i1 = -xdif1 / (xdif2 - xdif1) * i2;
      i2 = i1 + 1;
    } else {
      while (i2 - i1 > 1) {
        int32_t im = (i1 + i2) / 2;
        double xdifm = xarr[im] - x;
        if (xdif1 * xdifm < 0.) {
          i2 = im;
        } else if (xdifm == 0.) {
          return yarr[im];
        } else {
          i1 = im;
          xdif1 = xdifm;
        }
      }
    }

    return table_interpolate(x, xarr[i1], xarr[i2], yarr[i1], yarr[i2],
                             in_meth);
  } else if (xdif1 == 0.) {
    return yarr[i1];
  } else if (xdif2 == 0.) {
    return yarr[i2];
  } else {
    if (xdif1 > 0) {
      i2 = i1 + 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], yarr[i1], yarr[i2],
                               lt_meth);
    } else {
      i1 = i2 - 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], yarr[i1], yarr[i2],
                               gt_meth);
    }
  }
};

Table1d::Table1d(int32_t _len)
    : len(_len), xarr(new double[_len]), yarr(new double[_len]){};

bool Table1d::validate(int32_t _len) {
  if (_len < 2) {
    ERROR("Table1d len can't be less than 2.")
    return false;
  }
  return true;
};

std::shared_ptr<Table1d> Table1d::construct(double *_xarr, double *_yarr,
                                            int32_t _len) {
  if (!validate(_len)) {
    return nullptr;
  }
  if (_xarr == nullptr || _yarr == nullptr) {
    ERROR("Table1d data array can't be nullptr.")
    return nullptr;
  }
  std::shared_ptr<Table1d> p(new Table1d(_len));
  memcpy(p->xarr, _xarr, sizeof(double) * _len);
  memcpy(p->yarr, _yarr, sizeof(double) * _len);
  return p;
};

std::shared_ptr<Table1d> Table1d::construct(std::istream &in) {
  char flag[TABLE_FLAG_SIZE];
  in.read(flag, TABLE_FLAG_SIZE);
  if (strcmp(flag, TABLE1D_FLAG) != 0) {
    ERROR("Not valid Table1d istream.")
    return nullptr;
  }
  int32_t _len;
  in.read(reinterpret_cast<char *>(&_len), sizeof(int32_t));
  if (!validate(_len)) {
    return nullptr;
  }
  std::shared_ptr<Table1d> p(new Table1d(_len));
  in.read(reinterpret_cast<char *>(&p->arith_seq), sizeof(bool));
  char c;
  in >> c;
  p->in_meth = static_cast<table_interpolation_method>(c);
  in >> c;
  p->lt_meth = static_cast<table_extrapolation_method>(c);
  in >> c;
  p->gt_meth = static_cast<table_extrapolation_method>(c);
  in.read(reinterpret_cast<char *>(p->xarr), sizeof(double) * _len);
  in.read(reinterpret_cast<char *>(p->yarr), sizeof(double) * _len);
  return p;
};

Table1d::~Table1d() {
  delete[] xarr;
  delete[] yarr;
};

std::ostream &operator<<(std::ostream &out, const Table1d &t) {
  out.write(TABLE1D_FLAG, TABLE_FLAG_SIZE);
  out.write(reinterpret_cast<const char *>(&t.len), sizeof(int32_t));
  out.write(reinterpret_cast<const char *>(&t.arith_seq), sizeof(bool));
  out << static_cast<char>(t.in_meth);
  out << static_cast<char>(t.lt_meth);
  out << static_cast<char>(t.gt_meth);
  out.write(reinterpret_cast<const char *>(t.xarr), sizeof(double) * t.len);
  out.write(reinterpret_cast<const char *>(t.yarr), sizeof(double) * t.len);
  return out;
};

void Table1d::print() const {
  std::cout << "Table1d[len=" << len << ";arith_seq=" << arith_seq
            << ";in_meth=" << in_meth << ";lt_meth=" << lt_meth
            << ";gt_meth=" << gt_meth << "]" << std::endl
            << "          x                         y           " << std::endl;
  for (int i = 0; i < len; i++) {
    printf("%#23.16g  %#23.16g\n", xarr[i], yarr[i]);
  }
};

double Table1d::operator()(double x) const {
  return table_search(x, xarr, yarr, len, arith_seq, in_meth, lt_meth, gt_meth);
};

Table2d::Table2d(int32_t _xlen, int32_t _ylen, bool _y2d, bool _z2d)
    : xlen(_xlen),
      ylen(_ylen),
      y2d(_y2d),
      z2d(_z2d),
      xarr(new double[_xlen]),
      yarr(_y2d ? new double[_xlen * _ylen] : new double[_ylen]),
      zarr(_z2d ? new double[_xlen * _ylen] : new double[_ylen]){};

bool Table2d::validate(int32_t _xlen, int32_t _ylen, bool _y2d, bool _z2d) {
  if (!_y2d && !_z2d) {
    ERROR("Table2d y2d and z2d can't be false at the same time")
    return false;
  }
  if (_xlen < 2 || _ylen < 2) {
    ERROR("Table2d xlen or ylen can't be less than 2.")
    return false;
  }
  return true;
};

std::shared_ptr<Table2d> Table2d::construct(double *_xarr, double *_yarr,
                                            double *_zarr, int32_t _xlen,
                                            int32_t _ylen, bool _y2d,
                                            bool _z2d) {
  if (!validate(_xlen, _ylen, _y2d, _z2d)) {
    return nullptr;
  }
  if (_xarr == nullptr || _yarr == nullptr || _zarr == nullptr) {
    ERROR("Table2d data array can't be nullptr.")
    return nullptr;
  }
  std::shared_ptr<Table2d> p(new Table2d(_xlen, _ylen, _y2d, _z2d));
  memcpy(p->xarr, _xarr, sizeof(double) * _xlen);
  memcpy(p->yarr, _yarr, sizeof(double) * (_y2d ? _xlen * _ylen : _ylen));
  memcpy(p->zarr, _zarr, sizeof(double) * (_z2d ? _xlen * _ylen : _ylen));
  return p;
};

std::shared_ptr<Table2d> Table2d::construct(std::istream &in) {
  char flag[TABLE_FLAG_SIZE];
  in.read(flag, TABLE_FLAG_SIZE);
  if (strcmp(flag, TABLE2D_FLAG) != 0) {
    ERROR("Not valid Table2d istream.")
    return nullptr;
  }
  int32_t _xlen, _ylen;
  bool _y2d, _z2d;
  in.read(reinterpret_cast<char *>(&_xlen), sizeof(int32_t));
  in.read(reinterpret_cast<char *>(&_ylen), sizeof(int32_t));
  in.read(reinterpret_cast<char *>(&_y2d), sizeof(bool));
  in.read(reinterpret_cast<char *>(&_z2d), sizeof(bool));
  if (!validate(_xlen, _ylen, _y2d, _z2d)) {
    return nullptr;
  }
  std::shared_ptr<Table2d> p(new Table2d(_xlen, _ylen, _y2d, _z2d));
  in.read(reinterpret_cast<char *>(&p->x_arith_seq), sizeof(bool));
  in.read(reinterpret_cast<char *>(&p->y_arith_seq), sizeof(bool));
  char c;
  in >> c;
  p->x_in_meth = static_cast<table_interpolation_method>(c);
  in >> c;
  p->x_lt_meth = static_cast<table_extrapolation_method>(c);
  in >> c;
  p->x_gt_meth = static_cast<table_extrapolation_method>(c);
  in >> c;
  p->y_in_meth = static_cast<table_interpolation_method>(c);
  in >> c;
  p->y_lt_meth = static_cast<table_extrapolation_method>(c);
  in >> c;
  p->y_gt_meth = static_cast<table_extrapolation_method>(c);
  in.read(reinterpret_cast<char *>(p->xarr), sizeof(double) * _xlen);
  in.read(reinterpret_cast<char *>(p->yarr),
          sizeof(double) * (_y2d ? _xlen * _ylen : _ylen));
  in.read(reinterpret_cast<char *>(p->zarr),
          sizeof(double) * (_z2d ? _xlen * _ylen : _ylen));
  return p;
};

Table2d::~Table2d() {
  delete[] xarr;
  delete[] yarr;
  delete[] zarr;
};

std::ostream &operator<<(std::ostream &out, const Table2d &t) {
  out.write(TABLE2D_FLAG, TABLE_FLAG_SIZE);
  out.write(reinterpret_cast<const char *>(&t.xlen), sizeof(int32_t));
  out.write(reinterpret_cast<const char *>(&t.ylen), sizeof(int32_t));
  out.write(reinterpret_cast<const char *>(&t.y2d), sizeof(bool));
  out.write(reinterpret_cast<const char *>(&t.z2d), sizeof(bool));
  out.write(reinterpret_cast<const char *>(&t.x_arith_seq), sizeof(bool));
  out.write(reinterpret_cast<const char *>(&t.y_arith_seq), sizeof(bool));
  out << static_cast<char>(t.x_in_meth);
  out << static_cast<char>(t.x_lt_meth);
  out << static_cast<char>(t.x_gt_meth);
  out << static_cast<char>(t.y_in_meth);
  out << static_cast<char>(t.y_lt_meth);
  out << static_cast<char>(t.y_gt_meth);
  out.write(reinterpret_cast<const char *>(t.xarr), sizeof(double) * t.xlen);
  out.write(reinterpret_cast<const char *>(t.yarr),
            sizeof(double) * (t.y2d ? t.xlen * t.ylen : t.ylen));
  out.write(reinterpret_cast<const char *>(t.zarr),
            sizeof(double) * (t.z2d ? t.xlen * t.ylen : t.ylen));
  return out;
};

void Table2d::print() const {
  std::cout << "Table2d[xlen=" << xlen << ";ylen=" << ylen << ";y2d=" << y2d
            << ";z2d=" << z2d << ";x_arith_seq=" << x_arith_seq
            << ";y_arith_seq=" << y_arith_seq << ";x_in_meth=" << x_in_meth
            << ";y_in_meth=" << y_in_meth << ";x_lt_meth=" << x_lt_meth
            << ";y_lt_meth=" << y_lt_meth << ";x_gt_meth=" << x_gt_meth
            << ";y_gt_meth=" << y_gt_meth << "]" << std::endl;
  if (!y2d) {
    printf("  x   |   z2d   |   y   |");
    for (int j = 0; j < ylen; j++) {
      printf(" %#23.16g", yarr[j]);
    }
    printf("\n------------------------|");
    for (int j = 0; j < ylen; j++) {
      printf("------------------------");
    }
    printf("\n");
  }

  if (!z2d) {
    printf("  x   |   y2d   |   z   |");
    for (int j = 0; j < ylen; j++) {
      printf(" %#23.16g", zarr[j]);
    }
    printf("\n------------------------|");
    for (int j = 0; j < ylen; j++) {
      printf("------------------------");
    }
    printf("\n");
  }

  for (int i = 0; i < xlen; i++) {
    printf("%#23.16g |", xarr[i]);
    if (y2d) {
      for (int j = 0; j < ylen; j++) {
        printf(" %#23.16g", yarr[i * ylen + j]);
      }
      if (z2d) {
        printf(" |");
      }
    }
    if (z2d) {
      for (int j = 0; j < ylen; j++) {
        printf(" %#23.16g", zarr[i * ylen + j]);
      }
    }
    printf("\n");
  }
};

double Table2d::operator()(double x, double y) const {
#define SEARCHY(ix)                                                         \
  table_search(y, y2d ? yarr + (ix)*ylen : yarr,                            \
               z2d ? zarr + (ix)*ylen : zarr, ylen, y_arith_seq, y_in_meth, \
               y_lt_meth, y_gt_meth)

  int i1 = 0;
  int i2 = xlen - 1;
  double xdif1 = xarr[i1] - x;
  double xdif2 = xarr[i2] - x;
  if (xdif1 * xdif2 < 0.) {
    if (x_arith_seq) {
      i1 = -xdif1 / (xdif2 - xdif1) * i2;
      i2 = i1 + 1;
    } else {
      while (i2 - i1 > 1) {
        int im = (i1 + i2) / 2;
        double xdifm = xarr[im] - x;
        if (xdif1 * xdifm < 0.) {
          i2 = im;
        } else if (xdifm == 0.) {
          return SEARCHY(im);
        } else {
          i1 = im;
          xdif1 = xdifm;
        }
      }
    }

    return table_interpolate(x, xarr[i1], xarr[i2], SEARCHY(i1), SEARCHY(i2),
                             x_in_meth);
  } else if (xdif1 == 0.) {
    return SEARCHY(i1);
  } else if (xdif2 == 0.) {
    return SEARCHY(i2);
  } else {
    if (xdif1 > 0.) {
      i2 = i1 + 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], SEARCHY(i1), SEARCHY(i2),
                               x_lt_meth);
    } else {
      i1 = i2 - 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], SEARCHY(i1), SEARCHY(i2),
                               x_gt_meth);
    }
  }

#undef SEARCHY
};

Table3d::Table3d(int32_t _len)
    : len(_len),
      xarr(new double[_len]),
      tarr(new std::shared_ptr<Table2d>[_len]) {}

std::shared_ptr<Table3d> Table3d::construct(double *_xarr,
                                            std::shared_ptr<Table2d> *_tarr,
                                            int _len) {
  if (!validate(_len)) {
    return nullptr;
  }
  if (_xarr == nullptr || _tarr == nullptr) {
    ERROR("Table3d data array can't be nullptr.")
    return nullptr;
  }
  std::shared_ptr<Table3d> p(new Table3d(_len));
  memcpy(p->xarr, _xarr, sizeof(double) * _len);
  for (int32_t i = 0; i < _len; i++) {
    if (_tarr[i] == nullptr) {
      ERROR("Table3d sub Table2d[" << i << "] can't be nullptr.")
      return nullptr;
    }
    p->tarr[i] = _tarr[i];
  }
  return p;
};

std::shared_ptr<Table3d> Table3d::construct(std::istream &in) {
  char flag[TABLE_FLAG_SIZE];
  in.read(flag, TABLE_FLAG_SIZE);
  if (strcmp(flag, TABLE3D_FLAG) != 0) {
    ERROR("Not valid Table3d istream.")
    return nullptr;
  }
  int32_t _len;
  in.read(reinterpret_cast<char *>(&_len), sizeof(int));
  if (!validate(_len)) {
    return nullptr;
  }
  std::shared_ptr<Table3d> p(new Table3d(_len));
  in.read(reinterpret_cast<char *>(&p->arith_seq), sizeof(bool));
  char c;
  in >> c;
  p->in_meth = static_cast<table_interpolation_method>(c);
  in >> c;
  p->lt_meth = static_cast<table_extrapolation_method>(c);
  in >> c;
  p->gt_meth = static_cast<table_extrapolation_method>(c);
  in.read(reinterpret_cast<char *>(p->xarr), sizeof(double) * _len);
  for (int32_t i = 0; i < _len; i++) {
    p->tarr[i] = Table2d::construct(in);
    if (p->tarr[i] == nullptr) {
      ERROR("Table3d sub Table2d[" << i << "] creation error.")
      return nullptr;
    }
  }
  return p;
};

Table3d::~Table3d() {
  delete[] xarr;
  delete[] tarr;
};

bool Table3d::validate(int32_t _len) {
  if (_len < 2) {
    ERROR("Table3d len can't be less than 2.")
    return false;
  }
  return true;
};

std::ostream &operator<<(std::ostream &out, const Table3d &t) {
  out.write(TABLE3D_FLAG, TABLE_FLAG_SIZE);
  out.write(reinterpret_cast<const char *>(&t.len), sizeof(int32_t));
  out.write(reinterpret_cast<const char *>(&t.arith_seq), sizeof(bool));
  out << static_cast<char>(t.in_meth);
  out << static_cast<char>(t.lt_meth);
  out << static_cast<char>(t.gt_meth);
  out.write(reinterpret_cast<const char *>(t.xarr), sizeof(double) * t.len);
  for (int i = 0; i < t.len; i++) out << *t.tarr[i];
  return out;
};

void Table3d::print() const {
  std::cout << "Table3d[len=" << len << ";arith_seq=" << arith_seq
            << ";in_meth=" << in_meth << ";lt_meth=" << lt_meth
            << ";gt_meth=" << gt_meth << "]" << std::endl;
  for (int i = 0; i < len; i++) {
    printf("x=%#.16g\n", xarr[i]);
    tarr[i]->print();
    std::cout << std::endl;
  }
};

double Table3d::operator()(double x, double y, double z) const {
  int i1 = 0;
  int i2 = len - 1;
  double xdif1 = xarr[i1] - x;
  double xdif2 = xarr[i2] - x;
  if (xdif1 * xdif2 < 0.) {
    if (arith_seq) {
      i1 = -xdif1 / (xdif2 - xdif1) * i2;
      i2 = i1 + 1;
    } else {
      while (i2 - i1 > 1) {
        int im = (i1 + i2) / 2;
        double xdifm = xarr[im] - x;
        if (xdif1 * xdifm < 0.) {
          i2 = im;
        } else if (xdifm == 0.) {
          return (*tarr[im])(y, z);
        } else {
          i1 = im;
          xdif1 = xdifm;
        }
      }
    }

    return table_interpolate(x, xarr[i1], xarr[i2], (*tarr[i1])(y, z),
                             (*tarr[i2])(y, z), in_meth);
  } else if (xdif1 == 0.) {
    return (*tarr[i1])(y, z);
  } else if (xdif2 == 0.) {
    return (*tarr[i2])(y, z);
  } else {
    if (xdif1 > 0.) {
      i2 = i1 + 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], (*tarr[i1])(y, z),
                               (*tarr[i2])(y, z), lt_meth);
    } else {
      i1 = i2 - 1;
      return table_extrapolate(x, xarr[i1], xarr[i2], (*tarr[i1])(y, z),
                               (*tarr[i2])(y, z), gt_meth);
    }
  }
};