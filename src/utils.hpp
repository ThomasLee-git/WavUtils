#pragma once

#include <cstdio>
#include <memory>
#include <string>

namespace wav_utils {
void check_condition(const bool cond, const std::string& err_msg);

template <class... Args>
std::string format_string(const char* format, Args... args) {
  const auto expected_len = std::snprintf(nullptr, 0, format, args...);
  if (expected_len < 0) {
    return std::string();
  }
  const auto len = static_cast<size_t>(expected_len) + 1;
  constexpr size_t kStackBufSize = 256;
  if (len <= kStackBufSize) {
    thread_local char buf[kStackBufSize];
    std::snprintf(buf, len, format, args...);
    return std::string(buf, buf + len - 1);
  }
  std::unique_ptr<char[]> heap_buf(new char[len]);
  std::snprintf(heap_buf.get(), len, format, args...);
  return std::string(heap_buf.get(), heap_buf.get() + len - 1);
}
}  // namespace wav_utils