// Just for fun noise experiments

#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string.h>
#include <stdint.h>
#include <vector>

void generate_white_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<unsigned short>& dist,
    std::vector<unsigned short>& samples) {
    for (size_t i = 0; i < samples.size(); i++)
        samples[i] = dist(engine);
}

void generate_brown_noise(
    std::mt19937& engine,
    std::vector<unsigned short>& samples) {

    float value = 65535.0f / 2;
    std::uniform_real_distribution<float> step_dist(-100, 100);
    float decay = 0.999f;

    for (size_t i = 0; i < samples.size(); i++) {
        value = value * decay + step_dist(engine);
        value = std::clamp(value, 0.0f, 65535.0f);
        samples[i] = (unsigned short)value;
    }
}

struct Vec2
{
    Vec2(float radians) : x(cos(radians)), y(sin(radians)) {}
    Vec2(float a, float b) : x(a), y(b) {}
    static float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
    float x, y;
};

// create a random gradient vector for a position.
// using a hash so the gradient vectors are reproducible
inline Vec2 get_gradient(int x, int y)
{
    uint32_t seed = x * 374761393u + y * 668265263u; // Large primes
    seed = (seed ^ (seed >> 13)) * 1274126177u;
    float angle = (seed & 0xFFFF) / 65535.0f * 2.0f * M_PI;
    return Vec2(angle);
}

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float perlin(Vec2 point)
{
    int x = floor(point.x);
    int y = floor(point.y);

    // get gradient vectors for each corner
    Vec2 tl = get_gradient(x, y);
    Vec2 tr = get_gradient(x + 1, y);
    Vec2 bl = get_gradient(x, y + 1);
    Vec2 br = get_gradient(x + 1, y + 1);

    // get the offset vectors (distance from the corners to the point)
    Vec2 offset_tl = Vec2(point.x - x, point.y - y);
    Vec2 offset_bl = Vec2(point.x - x, point.y - (y + 1));
    Vec2 offset_tr = Vec2(point.x - (x + 1), point.y - y);
    Vec2 offset_br = Vec2(point.x - (x + 1), point.y - (y + 1));

    // smooth the fractional position
    float u = fade(point.x - x);
    float v = fade(point.y - y);

    // interpolate the position between the the scalar influence values from each corner
    float a = std::lerp(Vec2::dot(tl, offset_tl), Vec2::dot(tr, offset_tr), u);
    float b = std::lerp(Vec2::dot(bl, offset_bl), Vec2::dot(br, offset_br), u);
    float noise = std::lerp(a, b, v);

    // clamp to a range of 0 to 1
    return (noise + 1.0) * 0.5f;
}

void generate_perlin_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<unsigned short>& dist,
    std::vector<unsigned short>& samples) {
    // TODO: what are some interesting ways to visualize its output?
    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = perlin(Vec2(dist(engine), 0)) * 65535;
    }
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

void write_wav_file(int audio_seconds)
{
    uint32_t frequency = 44100; // samples per second
    uint32_t num_samples = frequency * audio_seconds;
    int bytes_per_sample = sizeof(unsigned short);
    uint32_t num_sample_bytes = num_samples * bytes_per_sample;

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<unsigned short> distribution(0, 65535);

    std::vector<unsigned short> samples;
    samples.resize(num_samples);
    //generate_white_noise(engine, distribution, samples);
    //generate_brown_noise(engine, samples);
    generate_perlin_noise(engine, distribution, samples);
    // TODO: simplex noise
    // TODO: worley noise
    // TODO: fractal noise

    WavHeader header = {
        .total_file_size = num_sample_bytes + 44,
        .fmt_chunk_size = 16,
        .audio_format = 1, // uncompressed samples
        .num_channels = 1,
        .sample_rate = frequency,
        .byte_rate = frequency * bytes_per_sample,
        .bytes_per_sample = (uint16_t)bytes_per_sample,
        .bits_per_sample = (uint16_t)(bytes_per_sample * 8),
        .data_chunk_size = num_sample_bytes,
    };
    memcpy(header.riff_chunk_id, "RIFF", 4);
    memcpy(header.riff_format, "WAVE", 4);
    memcpy(header.fmt_chunk_id, "fmt ", 4);
    memcpy(header.data_chunk_id, "data", 4);

    std::ofstream output("perlin.wav", std::ios::binary | std::ios::out);
    output.write((char*)&header, sizeof(WavHeader));
    output.write((char*)samples.data(), samples.size() * sizeof(unsigned short));
    output.close();
}

int main()
{
    write_wav_file(10);
}
