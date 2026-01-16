#include "wav_header.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace wav_utils {

void to_string(ByteType const* data, int const n, std::string& result) {
  std::ostringstream oss;
  for (auto idx = 0; idx < n; ++idx) {
    oss << data[idx];
  }
  result = oss.str();
  return;
}

int find_pos(ByteType const* data,
             int const l_idx,
             int const r_idx,
             const std::string& target,
             int const n) {
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

FmtChunk::FmtChunk(ByteType const* data, std::int32_t const size) {
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

FactChunk::FactChunk(ByteType const* data, std::int32_t const size) {
  auto pos = 0;
  auto n = 4;
  to_int(&(data[pos]), n, _sample_length);
  pos += n;
}

DataChunk::DataChunk(ByteType const* data, std::int32_t const size) {
  data_ = data;
  size_ = size;
}

void DataChunk::read(std::vector<std::int16_t>& data,
                     const std::int16_t num_bytes_per_sample) const {
  for (auto idx = 0; idx < data.size(); ++idx) {
    to_int(&data_[idx * num_bytes_per_sample], num_bytes_per_sample, data[idx]);
  }
}
void DataChunk::read(std::vector<std::int32_t>& data,
                     const std::int16_t num_bytes_per_sample) const {
  for (auto idx = 0; idx < data.size(); ++idx) {
    to_int(&data_[idx * num_bytes_per_sample], num_bytes_per_sample, data[idx]);
  }
}

class WavReader::Impl {
 public:
  Impl() = delete;
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  Impl(const std::vector<ByteType>& data, const bool split_channel);

  const FmtChunk& fmtChunk() const { return *fmt_chunk_; }
  const DataChunk& dataChunk() const { return *data_chunk_; }

  const std::int32_t numSamplesPerChannel() const {
    return num_samples_per_channel_;
  }
  const float* contiguousReadPointer() const { return data_float_.data(); }

 private:
  std::unique_ptr<FmtChunk> fmt_chunk_ = nullptr;
  std::unique_ptr<DataChunk> data_chunk_ = nullptr;

  std::int16_t num_bytes_per_sample_;
  std::int32_t num_total_samples_;
  std::int32_t num_samples_per_channel_;

  std::vector<std::int16_t> data_16_;
  std::vector<std::int32_t> data_32_;
  std::vector<float> data_float_;
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
  auto l_idx = pos;
  for (auto chunk_id : chunk_ids) {
    n = 4;
    // find chunk_id_pos
    auto chunk_pos = find_pos(data.data(), l_idx, data.size(), chunk_id, n);
    if (chunk_pos < 0) {
      const auto err_msg =
          format_string("failed finding chunk_id=%s", chunk_id.c_str());
      throw std::runtime_error(err_msg);
    }
    chunk_pos += n;
    // read chunk_size
    n = 4;
    std::int32_t chunk_size;
    to_int<std::int32_t>(&(*(data.begin() + chunk_pos)), n, chunk_size);
    chunk_pos += n;
    // create chunk
    if ("fmt " == chunk_id) {
      fmt_chunk_ = std::make_unique<FmtChunk>(&(data[chunk_pos]), chunk_size);
    } else if ("data" == chunk_id) {
      data_chunk_ = std::make_unique<DataChunk>(&(data[chunk_pos]), chunk_size);
    } else {
      const std::string err_msg =
          format_string("unknown chunk_id=", chunk_id.c_str());
      throw std::runtime_error(err_msg);
    }
    l_idx += chunk_pos + chunk_size;
  }
  // read data
  num_bytes_per_sample_ =
      fmt_chunk_->numBitsPerSample() / wav_utils::constants::num_bits_per_byte;
  num_total_samples_ = data_chunk_->numBytes() / num_bytes_per_sample_;
  num_samples_per_channel_ = num_total_samples_ / fmt_chunk_->numChannels();
  data_float_.resize(num_total_samples_);
  switch (fmt_chunk_->numBitsPerSample()) {
    case (16): {
      data_16_.resize(num_total_samples_);
      data_chunk_->read(data_16_, num_bytes_per_sample_);
      std::transform(data_16_.cbegin(), data_16_.cend(), data_float_.begin(),
                     [](std::int16_t v) { return float(v) / INT16_MAX; });
      break;
    }
    case (24): {
      data_32_.resize(num_total_samples_);
      data_chunk_->read(data_32_, num_bytes_per_sample_);
      std::transform(data_32_.cbegin(), data_32_.cend(), data_float_.begin(),
                     [](std::int16_t v) { return float(v) / INT32_MAX; });
      break;
    }
    case (32): {
      data_32_.resize(num_total_samples_);
      data_chunk_->read(data_32_, num_bytes_per_sample_);
      std::transform(data_32_.cbegin(), data_32_.cend(), data_float_.begin(),
                     [](std::int16_t v) { return float(v) / INT32_MAX; });
      break;
    }
    default: {
      const auto err_msg = format_string("unsupported bit_depth=%d",
                                         fmt_chunk_->numBitsPerSample());
      throw std::runtime_error(err_msg);
      break;
    }
  }
}

WavReader::WavReader(const std::vector<ByteType>& data,
                     const bool split_channel) {
  impl_ = std::make_unique<Impl>(data, split_channel);
}

WavReader::~WavReader() = default;

const std::int16_t WavReader::numChannels() const {
  return impl_->fmtChunk().numChannels();
}
const std::int32_t WavReader::sampleRate() const {
  return impl_->fmtChunk().sampleRate();
}
const std::int16_t WavReader::numBitsPerSample() const {
  return impl_->fmtChunk().numBitsPerSample();
}
const std::int32_t WavReader::numSamples() const {
  return impl_->numSamplesPerChannel();
}
const float* WavReader::contiguousReadPointer() const {
  return impl_->contiguousReadPointer();
}

}  // namespace wav_utils