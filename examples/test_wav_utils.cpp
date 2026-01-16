#include "file_reader.hpp"

#include <string>

int main(int argc, char* argv[]) {
  wav_utils::FileReader file_reader;
  file_reader.from_path(std::string(argv[1]));
}