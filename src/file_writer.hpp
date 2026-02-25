#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace wav_utils {

class FileWriter {
 public:
  FileWriter();
  ~FileWriter();
  FileWriter(const FileWriter&) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter(FileWriter&&) = delete;
  FileWriter& operator=(FileWriter&&) = delete;

  void toPath(const std::string& path,
              const std::vector<float>& data,
              const std::int16_t num_channels,
              const std::int32_t num_samples,
              const std::int32_t sample_rate,
              const std::int16_t num_bits_per_sample) const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace wav_utils
