// Just for fun noise experiments

#include <fstream>
#include <iostream>
#include <random>
#include <string.h>
#include <stdint.h>
#include <vector>

std::vector<unsigned short> white_noise_audio(int num_samples)
{
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<unsigned short> distribution(0, 65535);

    std::vector<unsigned short> samples;
    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; i++)
        samples.push_back(distribution(engine));
    return samples;
}

struct WavHeader
{
    // riff header
    uint8_t riff_chunk_id[4];
    uint32_t total_file_size;
    uint8_t riff_format[4];
    // fmt chunk
    uint8_t fmt_chunk_id[4];
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t bytes_per_sample;
    uint16_t bits_per_sample;
    // data chunk
    uint8_t data_chunk_id[4];
    uint32_t data_chunk_size;
};

void write_wav_file(std::vector<unsigned short>& samples)
{
    WavHeader header = {
        .total_file_size = (unsigned int)samples.size() * 2 + 44,
        .fmt_chunk_size = 16,
        .audio_format = 1, // uncompressed samples
        .num_channels = 1,
        .sample_rate = 44100,
        .byte_rate = 44100 * 2,
        .bytes_per_sample = 2,
        .bits_per_sample = 16,
        .data_chunk_size = (unsigned int)samples.size() * 2,
    };
    memcpy(header.riff_chunk_id, "RIFF", 4);
    memcpy(header.riff_format, "WAVE", 4);
    memcpy(header.fmt_chunk_id, "fmt ", 4);
    memcpy(header.data_chunk_id, "data", 4);

    std::ofstream output("output.wav", std::ios::binary | std::ios::out);
    output.write((char*)&header, sizeof(WavHeader));
    output.write((char*)samples.data(), samples.size() * sizeof(unsigned short));
    output.close();
}

int main()
{
    auto noise = white_noise_audio(44100 * 10);
    write_wav_file(noise);
}
