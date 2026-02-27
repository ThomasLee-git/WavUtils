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

  void fromPath(const std::string& path, const bool split_channel);
  void fromData(const std::vector<ByteType>& data, const bool split_channel);

  const std::int16_t numChannels() const { return _reader->numChannels(); }
  const std::int32_t sampleRate() const { return _reader->sampleRate(); }
  const std::int16_t numBitsPerSample() const {
    return _reader->numBitsPerSample();
  }
  const std::int32_t numSamples() const { return _reader->numSamples(); }
  const float* contiguousReadPointer() const {
    return _reader->contiguousReadPointer();
  }

 private:
  bool _initiated = false;
  std::unique_ptr<WavReader> _reader;
};

void FileReader::Impl::fromPath(const std::string& path,
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
  std::vector<ByteType> data(static_cast<std::size_t>(file_size));
  rbf.read(reinterpret_cast<char*>(data.data()),
           static_cast<std::streamsize>(file_size));
  if (rbf.gcount() != static_cast<std::streamsize>(file_size)) {
    throw std::runtime_error("failed reading file: read size mismatch");
  }
  return this->fromData(data, split_channel);
}
void FileReader::Impl::fromData(const std::vector<ByteType>& data,
                                const bool split_channel) {
  _reader = std::make_unique<WavReader>(data, split_channel);
}

FileReader::FileReader() {
  _impl = std::make_unique<Impl>();
}
FileReader::~FileReader() = default;

void FileReader::fromPath(const std::string& path, const bool split_channel) {
  return _impl->fromPath(path, split_channel);
}

const std::int16_t FileReader::numChannels() const {
  return _impl->numChannels();
}
const std::int32_t FileReader::sampleRate() const {
  return _impl->sampleRate();
}
const std::int16_t FileReader::numBitsPerSample() const {
  return _impl->numBitsPerSample();
}
const std::int32_t FileReader::numSamples() const {
  return _impl->numSamples();
}
const float* FileReader::contiguousReadPointer() const {
  return _impl->contiguousReadPointer();
}

}  // namespace wav_utils