#include "file_reader.hpp"
#include "utils.hpp"

#include <string>

int main(int argc, char* argv[]) {
  wav_utils::FileReader file_reader;
  file_reader.from_path(std::string(argv[1]));
  const auto msg = wav_utils::format_string(
      "num_channels: %d, bit_depth: %d, sample_rate: %d, num_samples: %d",
      file_reader.numChannels(), file_reader.numBitsPerSample(),
      file_reader.sampleRate(), file_reader.numSamples());
  return 0;
}