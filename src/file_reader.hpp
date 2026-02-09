#pragma once

#include <memory>
#include <string>

namespace wav_utils {

class FileReader {
 public:
  FileReader();
  ~FileReader();
  FileReader(const FileReader&) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader(FileReader&&) = delete;
  FileReader& operator=(FileReader&&) = delete;

  // read data
  void from_path(const std::string& path, const bool split_channel = false);

  // get info
  const std::int16_t numChannels() const;
  const std::int32_t sampleRate() const;
  const std::int16_t numBitsPerSample() const;
  const std::int32_t numSamples() const;
  const float* contiguousReadPointer() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace wav_utils