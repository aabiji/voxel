#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string.h>
#include <stdint.h>
#include <vector>

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
inline float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }
struct vec2 { float x, y; };

vec2 gradient(int x, int y)
{
    // xxHash
    int h = x * 3266489917 + y * 668265263;
    h ^= h >> 15;
    h *= 2246822519;
    h ^= h >> 13;
    h *= 3266489917;
    h ^= h >> 16;
    // random gradient vector
    vec2 directions[] = {
        vec2{1, 0}, vec2{-1, 0}, vec2{0, 1}, vec2{0, -1},
        vec2{1, 1}, vec2{-1, 1}, vec2{1, -1}, vec2{-1, -1}
    };
    return directions[h & 7];
}

float perlin(float x, float y)
{
    // get the grid cell the point's in and
    // the direction of the point in that grid cell
    int X = std::floor(x);
    int Y = std::floor(y);
    float dx = x - X;
    float dy = y - Y;

    // get the gradient vectors for each corner
    vec2 gtl = gradient(X, Y);
    vec2 gtr = gradient(X + 1, Y);
    vec2 gbl = gradient(X, Y + 1);
    vec2 gbr = gradient(X + 1, Y + 1);

    // compute dot products to get the gradient values for each corner
    float vtl = gtl.x * dx       + gtl.y * dy;
    float vtr = gtr.x * (dx - 1) + gtr.y * dy;
    float vbl = gbl.x * dx       + gbl.y * (dy - 1);
    float vbr = gbr.x * (dx - 1) + gbr.y * (dy - 1);

    // interpolate those values
    float a = lerp(vtl, vtr, fade(dx));
    float b = lerp(vbl, vbr, fade(dx));
    float noise = lerp(a, b, fade(dy));
    return noise * 0.7f + 0.5f;
}

float simplex(float x, float y)
{
    // get the coordinate of the simplex cell the point's in
    float skew = 0.5f * (sqrt(3.0f) - 1.0f);
    int i = std::floor(x + (x + y) * skew);
    int j = std::floor(y + (x + y) * skew);

    // unskew back to (x, y) space
    float unskew = (3.0f - sqrt(3.0f)) / 6.0f;
    float x0 = x - (i - (i + j) * unskew);
    float y0 = y - (j - (i + j) * unskew);

    // determine the simplex the point's in (upper or lower triangle)
    int i1 = x0 > y0 ? 1 : 0;
    int j1 = x0 > y0 ? 0 : 1;

    // get the gradients for the corner
    vec2 g0 = gradient(i, j);
    vec2 g1 = gradient(i + i1, j + j1);
    vec2 g2 = gradient(i + 1, j + 1);

    // get the corner coordinates
    float x1 = x0 - i1 + unskew;
    float y1 = y0 - j1 + unskew;
    float x2 = x0 - 1.0f + 2.0f * unskew;
    float y2 = y0 - 1.0f + 2.0f * unskew;

    // sum the contributions from each corner
    float sum = 0.0f;

    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 > 0) sum += t0 * t0 * t0 * t0 * (g0.x * x0 + g0.y * y0);

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 > 0) sum += t1 * t1 * t1 * t1 * (g1.x * x1 + g1.y * y1);

    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 > 0) sum += t2 * t2 * t2 * t2 * (g2.x * x2 + g2.y * y2);

    // normalize to a range of 0 to 1
    return 70.0f * sum * 0.5f + 0.5f;
}

void generate_white_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<int16_t>& dist,
    std::vector<int16_t>& samples) {
    for (size_t i = 0; i < samples.size(); i++)
        samples[i] = dist(engine);
}

void generate_brown_noise(std::mt19937& engine, std::vector<int16_t>& samples) {
    float value = 0;
    float decay = 0.999f;
    std::uniform_real_distribution<float> step_dist(-100, 100);

    for (size_t i = 0; i < samples.size(); i++) {
        value = value * decay + step_dist(engine);
        value = std::clamp(value, -32767.0f, 32767.0f);
        samples[i] = (int16_t)value;
    }
}

void generate_perlin_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<int16_t>& dist,
    std::vector<int16_t>& samples) {
    float frequency = 440;
    float sampling_rate = 44100;
    for (size_t i = 0; i < samples.size(); i++) {
        float t = (i / sampling_rate) * frequency;
        //samples[i] = (int16_t)(-32767.0f + perlin(t, 0) * 65535.0f);
        samples[i] = (int16_t)(-32767.0f + simplex(t, 0) * 65535.0f);
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
    int bytes_per_sample = sizeof(int16_t);
    uint32_t num_sample_bytes = num_samples * bytes_per_sample;

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<int16_t> distribution(-32767, 32767);

    std::vector<int16_t> samples;
    samples.resize(num_samples);
    //generate_white_noise(engine, distribution, samples);
    //generate_brown_noise(engine, samples);
    generate_perlin_noise(engine, distribution, samples);
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
    output.write((char*)samples.data(), samples.size() * sizeof(int16_t));
    output.close();
}

int main()
{
    write_wav_file(10);
}
