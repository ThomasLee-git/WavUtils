#include "file_reader.hpp"
#include "file_writer.hpp"
#include "utils.hpp"

#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  wav_utils::FileReader file_reader;
  file_reader.fromPath(std::string(argv[1]));
  const auto msg = wav_utils::format_string(
      "num_channels: %d, bit_depth: %d, sample_rate: %d, num_samples: %d",
      file_reader.numChannels(), file_reader.numBitsPerSample(),
      file_reader.sampleRate(), file_reader.numSamples());

  auto read_pointer = file_reader.contiguousReadPointer();
  const std::vector<float> data(
      read_pointer,
      read_pointer + file_reader.numChannels() * file_reader.numSamples());
  wav_utils::FileWriter file_writer;
  file_writer.toPath(std::string(argv[2]), data, file_reader.numChannels(),
                     file_reader.numSamples(), file_reader.sampleRate(),
                     file_reader.numBitsPerSample());
  return 0;
}