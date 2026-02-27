#include "wav_header.hpp"
#include "utils.hpp"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace wav_utils {

template <class T>
void to_int(const ByteType* data, const int n, T& result) {
  auto idx = 0;
  result = 0;
  do {
    result |= data[idx] << (wav_utils::constants::num_bits_per_byte * idx);
    idx += 1;
  } while (idx < n);
  return;
}
template void to_int<std::int32_t>(const ByteType* data,
                                   const int n,
                                   std::int32_t& result);
template void to_int<std::int16_t>(const ByteType* data,
                                   const int n,
                                   std::int16_t& result);

template <class T>
void from_int(const T value, const int n, ByteType* data) {
  for (auto idx = 0; idx < n; ++idx) {
    data[idx] = static_cast<ByteType>(
        (value >> (wav_utils::constants::num_bits_per_byte * idx)) & 0xFF);
  }
  return;
}
template void from_int<std::int32_t>(std::int32_t value,
                                     const int n,
                                     ByteType* data);
template void from_int<std::int16_t>(std::int16_t value,
                                     const int n,
                                     ByteType* data);

void to_string(const ByteType* data, const int n, std::string& result) {
  std::ostringstream oss;
  for (auto idx = 0; idx < n; ++idx) {
    oss << data[idx];
  }
  result = oss.str();
  return;
}

int find_pos(const ByteType* data,
             const int l_idx,
             const int r_idx,
             const std::string& target,
             const int n) {
  auto pos = l_idx;
  while (pos < r_idx) {
    if (std::memcmp(&(data[pos]), target.data(), n) == 0) {
      break;
    }
    pos += 1;
  }
  if (pos >= r_idx) {
    return -1;
  } else {
    return pos;
  }
}

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

  FmtChunk(const ByteType* data, const std::int32_t size);
  FmtChunk(const bool dummy);

  void toBytes(const std::int16_t num_channels,
               const std::int32_t num_samples_per_second,
               const std::int16_t num_bits_per_sample,
               std::vector<std::vector<ByteType>>& out) const;

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

FmtChunk::FmtChunk(const ByteType* data, const std::int32_t size) {
  _has_extension = false;
  auto pos = 0;
  auto n = 2;
  // format_tag
  std::int16_t format_tag_val;
  to_int(&(data[pos]), n, format_tag_val);
  _format_tag = FormatTag(format_tag_val);
  // num_channels
  pos += n;
  n = 2;
  to_int(&(data[pos]), n, _num_channels);
  // num_samples_per_second
  pos += n;
  n = 4;
  to_int(&(data[pos]), n, _num_samples_per_second);
  // num_avg_bytes_per_second
  pos += n;
  n = 4;
  to_int(&(data[pos]), n, _num_avg_bytes_per_second);
  // num_bytes_per_data_block
  pos += n;
  n = 2;
  to_int(&(data[pos]), n, _num_bytes_per_data_block);
  // num_bits_per_sample
  pos += n;
  n = 2;
  to_int(&(data[pos]), n, _num_bits_per_sample);
  // num_bytes_extension
  pos += n;
  if (pos < size) {
    n = 2;
    to_int(&(data[pos]), n, _num_bytes_extension);
    if (_num_bytes_extension > 0) {
      _has_extension = true;
      // num_valid_bits
      pos += n;
      n = 2;
      to_int(&(data[pos]), n, _num_valid_bits);
      // speaker_position_mask
      pos += n;
      n = 4;
      to_int(&(data[pos]), n, _speaker_position_mask);
      // sub_format
      pos += n;
      n = 4;
      to_string(&(data[pos]), n, _sub_format);
    }
  }
  return;
}

FmtChunk::FmtChunk(const bool dummy) {
  return;
}

void FmtChunk::toBytes(const std::int16_t num_channels,
                       const std::int32_t num_samples_per_second,
                       const std::int16_t num_bits_per_sample,
                       std::vector<std::vector<ByteType>>& out) const {
  // calculate
  const auto format_tag = FormatTag::WAVE_FORMAT_PCM;
  const std::int32_t num_avg_bytes_per_second =
      num_samples_per_second * num_channels * num_bits_per_sample /
      wav_utils::constants::num_bits_per_byte;
  const int16_t num_bytes_per_data_block =
      num_channels * num_bits_per_sample /
      wav_utils::constants::num_bits_per_byte;

  // TODO: add extension

  // create
  out.resize(3);
  out[0] = {'f', 'm', 't', ' '};
  auto& size = out[1];
  size.resize(4);
  auto& body = out[2];
  out[2].resize(16);
  // fmt chunk body
  auto pos = 0;
  from_int(static_cast<std::int16_t>(format_tag), 2, &body[pos]);
  pos += 2;
  from_int(num_channels, 2, &body[pos]);
  pos += 2;
  from_int(num_samples_per_second, 4, &body[pos]);
  pos += 4;
  from_int(num_avg_bytes_per_second, 4, &body[pos]);
  pos += 4;
  from_int(num_bytes_per_data_block, 2, &body[pos]);
  pos += 2;
  from_int(num_bits_per_sample, 2, &body[pos]);
  pos += 2;

  // fill size
  from_int(static_cast<std::int32_t>(body.size()), size.size(), size.data());
  return;
}
class FactChunk {
 public:
  FactChunk() = delete;
  ~FactChunk() = default;
  FactChunk(const FactChunk&) = delete;
  FactChunk& operator=(const FactChunk&) = delete;
  FactChunk(FactChunk&&) = delete;
  FactChunk& operator=(FactChunk&&) = delete;

  FactChunk(const ByteType* data, const std::int32_t size);

 private:
  std::int32_t _sample_length;
};

FactChunk::FactChunk(const ByteType* data, const std::int32_t size) {
  auto pos = 0;
  auto n = 4;
  to_int(&(data[pos]), n, _sample_length);
  pos += n;
}

class DataChunk {
 public:
  DataChunk() = delete;
  ~DataChunk() = default;
  DataChunk(const DataChunk&) = delete;
  DataChunk& operator=(const DataChunk&) = delete;
  DataChunk(DataChunk&&) = delete;
  DataChunk& operator=(DataChunk&&) = delete;

  DataChunk(const ByteType* data, const std::int32_t size);
  DataChunk(const bool dummy);
  void toBytes(const float* data,
               const std::int32_t num_total_samples,
               const std::int16_t num_bits_per_sample,
               std::vector<std::vector<ByteType>>& out) const;

  const std::int32_t numBytes() const { return _size; }
  void read(const std::int32_t num_total_samples,
            const std::int16_t num_bytes_per_sample,
            std::vector<float>& out) const;

 private:
  const ByteType* _data;
  std::int32_t _size;
};

DataChunk::DataChunk(const ByteType* data, const std::int32_t size) {
  _data = data;
  _size = size;
}

DataChunk::DataChunk(const bool dummy) {
  return;
}

void DataChunk::toBytes(const float* data,
                        const std::int32_t num_total_samples,
                        const std::int16_t num_bits_per_sample,
                        std::vector<std::vector<ByteType>>& out) const {
  const auto num_bytes_per_sample =
      num_bits_per_sample / wav_utils::constants::num_bits_per_byte;

  // init
  out.resize(3);
  out[0] = {'d', 'a', 't', 'a'};
  auto& size = out[1];
  size.resize(4);
  auto& body = out[2];
  body.resize(num_total_samples * num_bytes_per_sample);

  // write
  switch (num_bits_per_sample) {
    case (16): {
      for (auto idx = 0; idx < num_total_samples; ++idx) {
        const auto v = std::max(-1.f, std::min(1.f, data[idx]));
        const std::int16_t s = static_cast<std::int16_t>(v * INT16_MAX);
        from_int(s, num_bytes_per_sample, &body[idx * num_bytes_per_sample]);
      }
      break;
    }
    case (32): {
      for (auto idx = 0; idx < num_total_samples; ++idx) {
        const auto v = std::max(-1.f, std::min(1.f, data[idx]));
        const std::int32_t s = static_cast<std::int32_t>(v * INT32_MAX);
        from_int(s, num_bytes_per_sample, &body[idx * num_bytes_per_sample]);
      }
      break;
    }
    default: {
      const auto err_msg =
          format_string("unsupported bit_depth=%d", num_bits_per_sample);
      throw std::runtime_error(err_msg);
      break;
    }
  }

  // fill size
  from_int(static_cast<std::int32_t>(body.size()), size.size(), size.data());

  return;
}

void DataChunk::read(const std::int32_t num_total_samples,
                     const std::int16_t num_bytes_per_sample,
                     std::vector<float>& out) const {
  switch (num_bytes_per_sample) {
    case (2): {
      for (auto idx = 0; idx < num_total_samples; ++idx) {
        std::int16_t tmp_v;
        to_int(&(_data[idx * num_bytes_per_sample]), num_bytes_per_sample,
               tmp_v);
        float v = float(tmp_v) / INT16_MAX;
        out[idx] = v;
      }
      break;
    }
    case (4): {
      for (auto idx = 0; idx < num_total_samples; ++idx) {
        std::int32_t tmp_v;
        to_int(&(_data[idx * num_bytes_per_sample]), num_bytes_per_sample,
               tmp_v);
        float v = float(tmp_v) / INT32_MAX;
        out[idx] = v;
      }
      break;
    }
    default: {
      const auto err_msg = format_string("unsupported num_bytes_per_sample=%d",
                                         num_bytes_per_sample);
      throw std::runtime_error(err_msg);
      break;
    }
  }
  return;
}

class WavReader::Impl {
 public:
  Impl() = delete;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  Impl(const std::vector<ByteType>& data, const bool split_channel);

  const FmtChunk& fmtChunk() const { return *_fmt_chunk; }
  const DataChunk& dataChunk() const { return *_data_chunk; }

  const std::int32_t numSamplesPerChannel() const {
    return _num_samples_per_channel;
  }
  const float* contiguousReadPointer() const { return _data_float.data(); }

 private:
  std::unique_ptr<FmtChunk> _fmt_chunk = nullptr;
  std::unique_ptr<DataChunk> _data_chunk = nullptr;

  std::int16_t _num_bytes_per_sample;
  std::int32_t _num_total_samples;
  std::int32_t _num_samples_per_channel;
  std::vector<float> _data_float;
};

WavReader::Impl::Impl(const std::vector<ByteType>& data,
                      const bool split_channel) {
  auto pos = 0;

  // read chunk_id
  int n = 4;
  const std::string chunk_id(data.begin() + pos, data.begin() + pos + n);
  check_condition(chunk_id == "RIFF",
                  format_string("unknown chunk_id=%s", chunk_id.c_str()));
  pos += n;
  // read chunk_size
  n = 4;
  std::int32_t total_chunk_size;
  to_int(&(*(data.begin() + pos)), n, total_chunk_size);
  pos += n;
  // add offset
  total_chunk_size += 4;
  // read wave_id
  n = 4;
  const std::string wave_id(data.begin() + pos, data.begin() + pos + n);
  check_condition(wave_id == "WAVE",
                  format_string("unknown wave_id=%s", wave_id.c_str()));
  pos += n;
  // read chunk
  const std::vector<std::string> chunk_ids{"fmt ", "data"};
  const auto len_to_search = 512;
  auto chunk_l_pos = data.begin() + pos;
  for (const auto& chunk_id : chunk_ids) {
    n = 4;
    // find position
    auto chunk_pos = std::search(chunk_l_pos, chunk_l_pos + len_to_search,
                                 chunk_id.begin(), chunk_id.end());
    if (chunk_pos >= chunk_l_pos + len_to_search) {
      const auto err_msg =
          format_string("failed finding chunk_id=%s", chunk_id.c_str());
      throw std::runtime_error(err_msg);
    }
    chunk_pos += n;
    // read chunk_size
    n = 4;
    std::int32_t chunk_size;
    to_int<std::int32_t>(&(*chunk_pos), n, chunk_size);
    chunk_pos += n;
    // create chunk
    if ("fmt " == chunk_id) {
      _fmt_chunk = std::make_unique<FmtChunk>(&(*chunk_pos), chunk_size);
    } else if ("data" == chunk_id) {
      _data_chunk = std::make_unique<DataChunk>(&(*chunk_pos), chunk_size);
    } else {
      const std::string err_msg =
          format_string("unknown chunk_id=", chunk_id.c_str());
      throw std::runtime_error(err_msg);
    }
    chunk_l_pos = chunk_pos + chunk_size;
  }
  // check all chunks are found
  if (_fmt_chunk == nullptr) {
    const auto err_msg =
        format_string("chunk_id=%s not found", chunk_ids[0].c_str());
    throw std::runtime_error(err_msg);
  }
  if (_data_chunk == nullptr) {
    const auto err_msg =
        format_string("chunk_id=%s not found", chunk_ids[1].c_str());
    throw std::runtime_error(err_msg);
  }
  // read data
  _num_bytes_per_sample =
      _fmt_chunk->numBitsPerSample() / wav_utils::constants::num_bits_per_byte;
  check_condition(_num_bytes_per_sample == 2 || _num_bytes_per_sample == 4,
                  format_string("unsupported bit_depth=%d",
                                _fmt_chunk->numBitsPerSample()));
  _num_total_samples = _data_chunk->numBytes() / _num_bytes_per_sample;
  _num_samples_per_channel = _num_total_samples / _fmt_chunk->numChannels();
  _data_float.resize(_num_total_samples);
  _data_chunk->read(_num_total_samples, _num_bytes_per_sample, _data_float);
}

WavReader::WavReader(const std::vector<ByteType>& data,
                     const bool split_channel) {
  _impl = std::make_unique<Impl>(data, split_channel);
}

WavReader::~WavReader() = default;

const std::int16_t WavReader::numChannels() const {
  return _impl->fmtChunk().numChannels();
}
const std::int32_t WavReader::sampleRate() const {
  return _impl->fmtChunk().sampleRate();
}
const std::int16_t WavReader::numBitsPerSample() const {
  return _impl->fmtChunk().numBitsPerSample();
}
const std::int32_t WavReader::numSamples() const {
  return _impl->numSamplesPerChannel();
}
const float* WavReader::contiguousReadPointer() const {
  return _impl->contiguousReadPointer();
}

// WavWriter

class WavWriter::Impl {
 public:
  Impl() = default;
  ~Impl() = default;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  void toBytes(const float* data,
               const std::int16_t num_channels,
               const std::int32_t num_samples,
               const std::int32_t sample_rate,
               const std::int16_t num_bits_per_sample,
               std::vector<std::vector<ByteType>>& out) const;
};

void WavWriter::Impl::toBytes(const float* data,
                              const std::int16_t num_channels,
                              const std::int32_t num_samples,
                              const std::int32_t sample_rate,
                              const std::int16_t num_bits_per_sample,
                              std::vector<std::vector<ByteType>>& out) const {
  out.resize(3);
  out[0] = {'R', 'I', 'F', 'F'};
  auto& size = out[1];
  size.resize(4);
  out[2] = {'W', 'A', 'V', 'E'};

  FmtChunk fmt_chunk(false);
  std::vector<std::vector<ByteType>> fmt_chunk_bytes;
  fmt_chunk.toBytes(num_channels, sample_rate, num_bits_per_sample,
                    fmt_chunk_bytes);
  DataChunk data_chunk(false);
  std::vector<std::vector<ByteType>> data_chunk_bytes;
  data_chunk.toBytes(data, num_samples * num_channels, num_bits_per_sample,
                     data_chunk_bytes);

  // push back
  for (auto& tmp_bytes : fmt_chunk_bytes) {
    out.push_back(std::move(tmp_bytes));
  }
  for (auto& tmp_bytes : data_chunk_bytes) {
    out.push_back(std::move(tmp_bytes));
  }

  // get total_size
  std::size_t total_size = 0;
  for (auto idx = 2; idx < out.size(); ++idx) {
    total_size += out[idx].size();
  }

  // fill size
  from_int(static_cast<std::int32_t>(total_size), size.size(), size.data());
  return;
}

WavWriter::WavWriter() {
  _impl = std::make_unique<Impl>();
}
WavWriter::~WavWriter() = default;

void WavWriter::toBytes(const float* data,
                        const std::int16_t num_channels,
                        const std::int32_t num_samples,
                        const std::int32_t sample_rate,
                        const std::int16_t num_bits_per_sample,
                        std::vector<std::vector<ByteType>>& out) const {
  _impl->toBytes(data, num_channels, num_samples, sample_rate,
                 num_bits_per_sample, out);
  return;
}

}  // namespace wav_utils