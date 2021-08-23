#include "Storage.hh"

#include "ProParticle.hh"

H5Storage::H5Storage(const std::string &_fileName,
                     const std::string &_storageItems, hsize_t length)
    : file(_fileName, H5F_ACC_TRUNC), currentSize{0, 0}, singleRowSize{1, 0} {
  std::string tmp;
  for (char c : _storageItems) {
    if (c == ',' || c == ' ') {
      if (!tmp.empty()) {
        storageItems.push_back(tmp);
        tmp.clear();
      }
    } else {
      tmp += c;
    }
  }
  if (!tmp.empty()) {
    storageItems.push_back(tmp);
  }
  currentSize[1] = length;
  singleRowSize[1] = length;
  singleRow = H5::DataSpace(2, singleRowSize);

  hsize_t maxSize[2] = {H5S_UNLIMITED, length};
  H5::DataSpace dataspace(2, currentSize, maxSize);
  H5::DSetCreatPropList prop;
  prop.setChunk(2, singleRowSize);
  prop.setDeflate(6);
  for (auto &key : storageItems) {
    data_sets[key] =
        file.createDataSet(key, H5::PredType::IEEE_F64LE, dataspace, prop);
  }
};
H5Storage::~H5Storage() {}

void H5Storage::writeTime(double *times) {
  H5::DataSet timeDataSet =
      file.createDataSet("t", H5::PredType::IEEE_F64LE, H5::DataSpace(1, &singleRowSize[1]));

  timeDataSet.write(times, H5::PredType::NATIVE_DOUBLE);
}

void H5Storage::write(const std::map<std::string, std::vector<double>> &map) {
  hsize_t offset[2] = {currentSize[0]++, 0};
  H5::DataSpace filespace(2, currentSize);
  filespace.selectHyperslab(H5S_SELECT_SET, singleRowSize, offset);
  for (auto &key : storageItems) {
    auto &data = map.at(key);
    auto &data_set = data_sets.at(key);
    data_set.extend(currentSize);
    data_set.write(data.data(), H5::PredType::NATIVE_DOUBLE, singleRow,
                   filespace);
  }
}