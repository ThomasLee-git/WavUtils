#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace wav_utils {

namespace constants {
constexpr int num_bits_per_byte = 8;
};

using ByteType = std::uint8_t;

template <class T>
void to_int(ByteType const* data, int const n, T& result);

void to_string(ByteType const* data, int const n, std::string& result);

int find_pos(ByteType const* data,
             int const l_idx,
             int const r_idx,
             const std::string& target,
             int const n);

class WavReader {
 public:
  WavReader() = delete;
  WavReader(const WavReader&) = delete;
  WavReader& operator=(const WavReader&) = delete;
  WavReader(WavReader&&) = delete;
  WavReader& operator=(WavReader&&) = delete;
  ~WavReader();

  WavReader(const std::vector<ByteType>& data, const bool split_channel);

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