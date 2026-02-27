#include "file_writer.hpp"
#include "utils.hpp"
#include "wav_header.hpp"

#include <cstdint>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace wav_utils {

class FileWriter::Impl {
 public:
  Impl() = default;
  ~Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  void toPath(const std::string& path,
              const float* data,
              const std::int16_t num_channels,
              const std::int32_t num_samples,
              const std::int32_t sample_rate,
              const std::int16_t num_bits_per_sample) const;
};

void FileWriter::Impl::toPath(const std::string& path,
                              const float* data,
                              const std::int16_t num_channels,
                              const std::int32_t num_samples,
                              const std::int32_t sample_rate,
                              const std::int16_t num_bits_per_sample) const {
  check_condition(
      num_channels > 0 && num_samples > 0 && num_bits_per_sample > 0,
      "setFormat must be called before toPath");
  check_condition(data != nullptr, "data must not be null");

  WavWriter writer;
  std::vector<std::vector<ByteType>> file_bytes;
  writer.toBytes(data, num_channels, num_samples, sample_rate,
                 num_bits_per_sample, file_bytes);

  std::ofstream wbf(path, std::ios::binary);
  if (!wbf.is_open()) {
    const auto err_msg = format_string("failed opening %s", path.c_str());
    throw std::runtime_error(err_msg);
  }
  wbf.seekp(0, std::ios::beg);
  for (const auto& tmp_bytes : file_bytes) {
    wbf.write(reinterpret_cast<const char*>(tmp_bytes.data()),
              static_cast<std::streamsize>(tmp_bytes.size()));
  }

  return;
}

FileWriter::FileWriter() {
  _impl = std::make_unique<Impl>();
}
FileWriter::~FileWriter() = default;

void FileWriter::toPath(const std::string& path,
                        const float* data,
                        const std::int16_t num_channels,
                        const std::int32_t num_samples,
                        const std::int32_t sample_rate,
                        const std::int16_t num_bits_per_sample) const {
  _impl->toPath(path, data, num_channels, num_samples, sample_rate,
                num_bits_per_sample);
}

}  // namespace wav_utils
