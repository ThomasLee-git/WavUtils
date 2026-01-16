#include "file_reader.hpp"
#include "utils.hpp"
#include "wav_header.hpp"

#include <cstdint>
#include <fstream>
#include <ios>
#include <memory>
#include <stdexcept>
#include <vector>

namespace wav_utils {

class FileReader::Impl {
 public:
  Impl() = default;
  ~Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  void from_path(const std::string& path, const bool split_channel);
  void from_data(const std::vector<ByteType>& data, const bool split_channel);

 private:
  bool _initiated = false;
  std::unique_ptr<WavReader> reader_;
};

void FileReader::Impl::from_path(const std::string& path,
                                 const bool split_channel) {
  std::ifstream rbf(path, std::ios::binary);
  if (!rbf.is_open()) {
    const auto err_msg = format_string("failed opening %s", path.c_str());
    throw std::runtime_error(err_msg);
  }
  //
  rbf.unsetf(std::ios::skipws);
  rbf.seekg(0, std::ios::end);
  const auto file_size = rbf.tellg();
  if (file_size < 0) {
    const auto err_msg = "failed reading file_size";
    throw std::runtime_error(err_msg);
  }
  // rewind
  rbf.seekg(0, std::ios::beg);
  std::vector<ByteType> data(file_size);
  rbf.read(reinterpret_cast<char*>(data.data()), file_size);
  return this->from_data(data, split_channel);
}
void FileReader::Impl::from_data(const std::vector<ByteType>& data,
                                 const bool split_channel) {
  reader_ = std::make_unique<WavReader>(data, split_channel);
}

FileReader::FileReader() {
  impl_ = std::make_unique<Impl>();
}
FileReader::~FileReader() = default;

void FileReader::from_path(const std::string& path, const bool split_channel) {
  return impl_->from_path(path, split_channel);
}

}  // namespace wav_utils