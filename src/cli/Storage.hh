#ifndef _SOVOL_STORAGE_HH
#define _SOVOL_STORAGE_HH 1

#include <map>
#include <vector>

#include "H5Cpp.h"

class H5Storage {
 private:
  H5::H5File file;
  std::vector<std::string> storageItems;
  std::map<std::string, H5::DataSet> data_sets;
  hsize_t currentSize[2];
  hsize_t singleRowSize[2];
  H5::DataSpace singleRow;

 public:
  H5Storage(const std::string &_fileName,
            const std::vector<std::string> &_storageItems, hsize_t length);
  void write(const std::map<std::string, std::vector<double>> &);
  void writeTime(double *);
};

#endif