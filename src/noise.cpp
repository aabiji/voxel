#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string.h>
#include <stdint.h>
#include <vector>

struct Vec2
{
    Vec2(float radians) : x(cos(radians)), y(sin(radians)) {}
    Vec2(float a, float b) : x(a), y(b) {}
    Vec2() : x(0), y(0) {}
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    static float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
    float x, y;
};

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
inline float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }

// create a random gradient vector for a position.
// using a hash so the gradient vectors are reproducible
inline Vec2 get_gradient(int x, int y)
{
    uint32_t seed = x * 374761393u + y * 668265263u; // Large primes
    seed = (seed ^ (seed >> 13)) * 1274126177u;
    float angle = (seed & 0xFFFF) / 65535.0f * 2.0f * M_PI;
    return Vec2(angle);
}

float perlin(float pointx, float pointy)
{
    // coordinate of grid cell the point's in
    int x = floor(pointx);
    int y = floor(pointy);

    // get gradient vectors for each corner
    Vec2 tl = get_gradient(x, y);
    Vec2 tr = get_gradient(x + 1, y);
    Vec2 bl = get_gradient(x, y + 1);
    Vec2 br = get_gradient(x + 1, y + 1);

    // get the offset vectors (distance from the corners to the point)
    Vec2 offset_tl = Vec2(pointx - x, pointy - y);
    Vec2 offset_bl = Vec2(pointx - x, pointy - (y + 1));
    Vec2 offset_tr = Vec2(pointx - (x + 1), pointy - y);
    Vec2 offset_br = Vec2(pointx - (x + 1), pointy - (y + 1));

    // smooth the fractional position
    float u = fade(pointx - x);
    float v = fade(pointy - y);

    // interpolate the position between the the scalar influence values from each corner
    float a = lerp(Vec2::dot(tl, offset_tl), Vec2::dot(tr, offset_tr), u);
    float b = lerp(Vec2::dot(bl, offset_bl), Vec2::dot(br, offset_br), u);

    // clamp noise to a range of 0 to 1
    return (lerp(a, b, v) + 1.0) * 0.5f;
}

// our implemention's wrong: https://github.com/SRombauts/SimplexNoise
float simplex(float pointx, float pointy)
{
    const float skew = 0.5 * (std::sqrt(3) - 1);
    const float unskew = 1.0f / 6.0f;

    // get the skewed point
    float skew_offset = (pointx + pointy) * skew;
    float skewed_x = pointx + skew_offset;
    float skewed_y = pointy + skew_offset;

    // get the skewed coordinate of the grid cell the point's in
    float x = std::floor(skewed_x);
    float y = std::floor(skewed_y);

    // get the skewed vertices in the simplex shape
    // the simplex shape is a triangle because we're doing simplex noise in 2d
    // the triangle's shape depends on the point
    Vec2 skewed_vertices[3] = {
        Vec2(x, y),
        skewed_x - x > skewed_y - y ? Vec2(x + 1, y) : Vec2(x, y + 1),
        Vec2(x + 1, y + 1)
    };

    float sum = 0;
    for (int i = 0; i < 3; i++) {
        // get the unkewed vertex
        Vec2 skewed = skewed_vertices[i];
        float unskew_offset = (skewed.x + skewed.y) * unskew;
        Vec2 vertex(skewed.x - unskew_offset, skewed.y - unskew_offset);

        Vec2 offset = Vec2(pointx, pointy) - vertex;
        Vec2 gradient = get_gradient(skewed.x, skewed.y);

        // get the radial falloff and the gradient's contribution
        float t = std::max(0.0f, 0.5f - Vec2::dot(offset, offset));
        float contribution = t * t * t * t * Vec2::dot(gradient, offset);
        sum += contribution;
    }

    return 0.5f * ((sum * 70.0f) + 1.0f);
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
