#include "utils.hpp"

#include <stdexcept>

namespace wav_utils {
void check_condition(const bool cond, const std::string& err_msg) {
  if (!cond) {
    throw std::runtime_error(err_msg);
  }
  return;
}
}  // namespace wav_utils