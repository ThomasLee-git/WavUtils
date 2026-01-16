#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

namespace wav_utils {

namespace constants {
constexpr int num_bits_per_byte = 8;
};

using ByteType = std::uint8_t;

template <class T>
void to_int(ByteType const* data, int const n, T& result) {
  // NOTE: wav use little_endian
  constexpr int shift = 8;
  auto idx = 0;
  result = 0;
  do {
    result |= data[idx] << (shift * idx);
    idx += 1;
  } while (idx < n);
  return;
}
template void to_int<std::int32_t>(ByteType const* data,
                                   int const n,
                                   std::int32_t& result);
template void to_int<std::int16_t>(ByteType const* data,
                                   int const n,
                                   std::int16_t& result);

void to_string(ByteType const* data, int const n, std::string& result);
int find_pos(ByteType const* data,
             int const l_idx,
             int const r_idx,
             const std::string& target,
             int const n);

enum class FormatTag {
  WAVE_FORMAT_PCM = 0x0001,
  WAVE_FORMAT_IEEE_FLOAT = 0x0003,
  WAVE_FORMAT_ALAW = 0x0006,
  WAVE_FORMAT_MULAW = 0x0007,
  WAVE_FORMAT_EXTENSIBLE = 0xFFFE,
};

class FmtChunk {
 public:
  FmtChunk() = delete;
  ~FmtChunk() = default;
  FmtChunk(const FmtChunk&) = delete;
  FmtChunk& operator=(const FmtChunk&) = delete;
  FmtChunk(FmtChunk&&) = delete;
  FmtChunk& operator=(FmtChunk&&) = delete;

  FmtChunk(ByteType const* data, std::int32_t const size);

  const std::int16_t numChannels() const { return _num_channels; }
  const std::int32_t sampleRate() const { return _num_samples_per_second; }
  const std::int16_t numBitsPerSample() const { return _num_bits_per_sample; }

 private:
  FormatTag _format_tag;
  std::int16_t _num_channels;
  std::int32_t _num_samples_per_second;
  std::int32_t _num_avg_bytes_per_second;
  std::int16_t _num_bytes_per_data_block;
  std::int16_t _num_bits_per_sample;

  bool _has_extension;
  std::int16_t _num_bytes_extension;
  std::int16_t _num_valid_bits;
  std::int32_t _speaker_position_mask;
  std::string _sub_format;
};

class FactChunk {
 public:
  FactChunk() = delete;
  ~FactChunk() = default;
  FactChunk(const FactChunk&) = delete;
  FactChunk& operator=(const FactChunk&) = delete;
  FactChunk(FactChunk&&) = delete;
  FactChunk& operator=(FactChunk&&) = delete;

  FactChunk(ByteType const* data, std::int32_t const size);

 private:
  std::int32_t _sample_length;
};

class DataChunk {
 public:
  DataChunk() = delete;
  ~DataChunk() = default;
  DataChunk(const DataChunk&) = delete;
  DataChunk& operator=(const DataChunk&) = delete;
  DataChunk(DataChunk&&) = delete;
  DataChunk& operator=(DataChunk&&) = delete;

  DataChunk(ByteType const* data, std::int32_t const size);

  const std::int32_t numBytes() const { return size_; }
  void read(std::vector<std::int16_t>& data,
            const std::int16_t num_bytes_per_sample) const;
  void read(std::vector<std::int32_t>& data,
            const std::int16_t num_bytes_per_sample) const;

 private:
  ByteType const* data_;
  std::int32_t size_;
};

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