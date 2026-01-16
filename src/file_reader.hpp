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

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace wav_utils