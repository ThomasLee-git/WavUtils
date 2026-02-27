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
void to_int(const ByteType* data, const int n, T& result);

template <class T>
void from_int(const T value, const int n, ByteType* data);

void to_string(const ByteType* data, const int n, std::string& result);

int find_pos(const ByteType* data,
             const int l_idx,
             const int r_idx,
             const std::string& target,
             const int n);

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
  std::unique_ptr<Impl> _impl;
};

class WavWriter {
 public:
  WavWriter();
  WavWriter(const WavWriter&) = delete;
  WavWriter& operator=(const WavWriter&) = delete;
  WavWriter(WavWriter&&) = delete;
  WavWriter& operator=(WavWriter&&) = delete;
  ~WavWriter();

  void toBytes(const float* data,
               const std::int16_t num_channels,
               const std::int32_t num_samples,
               const std::int32_t sample_rate,
               const std::int16_t num_bits_per_sample,
               std::vector<std::vector<ByteType>>& out) const;

 private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

}  // namespace wav_utils