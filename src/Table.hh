#ifndef _SOVOL_TABLE_HH
#define _SOVOL_TABLE_HH 1

#include <iostream>
#include <memory>

#define TABLE1D_FLAG "TABLE1D"
#define TABLE2D_FLAG "TABLE2D"
#define TABLE3D_FLAG "TABLE3D"
#define TABLE_FLAG_SIZE 8

enum table_extrapolation_method {
  TABLE_NO_EXTRA,
  TABLE_CONSTANT_EXTRA,
  TABLE_LINEAR_EXTRA,
  TABLE_LOG_EXTRA,
  TABLE_EXP_EXTRA,
  TABLE_POWER_EXTRA
};

enum table_interpolation_method {
  TABLE_LINEAR_INTER,
  TABLE_LOG_INTER,
  TABLE_EXP_INTER,
  TABLE_POWER_INTER
};

class Table1d {
 private:
  int32_t len;
  double *xarr, *yarr;

  Table1d(int32_t _len);
  static bool validate(int32_t _len);

 public:
  bool arith_seq;
  table_interpolation_method in_meth;
  table_extrapolation_method lt_meth, gt_meth;

  ~Table1d();
  static std::shared_ptr<Table1d> construct(double *_xarr, double *_yarr,
                                            int32_t _len);
  static std::shared_ptr<Table1d> construct(std::istream &i);
  static std::shared_ptr<Table1d> construct(std::istream &&i) {
    return construct(i);
  };
  friend std::ostream &operator<<(std::ostream &o, const Table1d &t);
  inline friend std::ostream &operator<<(std::ostream &&o, const Table1d &t) {
    return o << t;
  };
  double operator()(double x) const;
  void print() const;
};

class Table2d {
 private:
  int32_t xlen, ylen;
  bool y2d, z2d;
  double *xarr, *yarr, *zarr;

  Table2d(int32_t _xlen, int32_t _ylen, bool _y2d, bool _z2d);
  static bool validate(int32_t _xlen, int32_t _ylen, bool _y2d, bool _z2d);

 public:
  bool x_arith_seq, y_arith_seq;
  table_interpolation_method x_in_meth, y_in_meth;
  table_extrapolation_method x_lt_meth, x_gt_meth, y_lt_meth, y_gt_meth;

  ~Table2d();
  static std::shared_ptr<Table2d> construct(double *_xarr, double *_yarr,
                                            double *_zarr, int32_t _xlen,
                                            int32_t _ylen, bool _y2d,
                                            bool _z2d);
  static std::shared_ptr<Table2d> construct(std::istream &i);
  static std::shared_ptr<Table2d> construct(std::istream &&i) {
    return construct(i);
  };
  friend std::ostream &operator<<(std::ostream &o, const Table2d &t);
  inline friend std::ostream &operator<<(std::ostream &&o, const Table2d &t) {
    return o << t;
  };
  double operator()(double x, double y) const;
  void print() const;
};

class Table3d {
 private:
  int32_t len;
  double *xarr;
  std::shared_ptr<Table2d> *tarr;

  Table3d(int32_t _len);
  static bool validate(int32_t _len);

 public:
  bool arith_seq;
  table_interpolation_method in_meth;
  table_extrapolation_method lt_meth, gt_meth;

  ~Table3d();
  static std::shared_ptr<Table3d> construct(double *_xarr,
                                            std::shared_ptr<Table2d> *_tarr,
                                            int32_t _len);
  static std::shared_ptr<Table3d> construct(std::istream &i);
  static std::shared_ptr<Table3d> construct(std::istream &&i) {
    return construct(i);
  };
  friend std::ostream &operator<<(std::ostream &o, const Table3d &t);
  inline friend std::ostream &operator<<(std::ostream &&o, const Table3d &t) {
    return o << t;
  };
  double operator()(double x, double y, double z) const;
  void print() const;
};

#endif