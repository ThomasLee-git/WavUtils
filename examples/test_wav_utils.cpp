#include "file_reader.hpp"
#include "file_writer.hpp"
#include "utils.hpp"

#include <string>

int main(int argc, char* argv[]) {
  wav_utils::FileReader file_reader;
  file_reader.fromPath(std::string(argv[1]));
  const auto msg = wav_utils::format_string(
      "num_channels: %d, bit_depth: %d, sample_rate: %d, num_samples: %d",
      file_reader.numChannels(), file_reader.numBitsPerSample(),
      file_reader.sampleRate(), file_reader.numSamples());

  const auto read_pointer = file_reader.contiguousReadPointer();
  wav_utils::FileWriter file_writer;
  file_writer.toPath(std::string(argv[2]), read_pointer,
                     file_reader.numChannels(), file_reader.numSamples(),
                     file_reader.sampleRate(), file_reader.numBitsPerSample());
  return 0;
}