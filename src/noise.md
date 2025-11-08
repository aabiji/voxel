# What do noise functions sound like?
[Abigail Adegbiji](https://aabiji.github.io/) • November 8, 2025

Lately I've been building a minecraft clone, and to do that I've needed
to think about how to procedurally generate terrain. A nice way to do
that would be to break the terrain into chunks then sample a noise
value to get the height values of the top layer voxels. That's made me
curious of different noise functions, how they work, and most importantly
what they sound like.

Let's start with the most obvious type of noise, white noise.
White noise is quite literally just a sequence of random frequencies.
You might implement a function to generate pcm samples of white noise like so:
```cpp
void generate_white_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<int16_t>& dist,
    std::vector<int16_t>& samples) {
    for (size_t i = 0; i < samples.size(); i++)
        samples[i] = dist(engine);
}
```

Of course, we'd want to listen to it, so we can easily
[write it to a wav file](https://www.hydrogen18.com/blog/joys-of-writing-a-wav-file.html).
Here we'll just use mono audio at 44100 Hz with 16 bit audio sampling.
```cpp
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
    // TODO: call noise generation function here

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

```

And as expected, we just get static.
<audio controls>
  <source src="white-noise.mp3" type="audio/mpeg">
</audio>

Another interesting noise function to look into is Brown noise.
Brown noise is the sounds like the equivalent of a
[drunken sailor](https://jamesclear.com/great-speeches/learning-to-learn-by-richard-hamming)
taking small, random steps, whilst drifting back to a spot.

TODO: actually explain how it works

```cpp
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
```
<audio controls>
  <source src="brown-noise.mp3" type="audio/mpeg">
</audio>

Brown noise sounds so much softer than white noise. The reason it's softer
is because of the decay, which acts as a low pass filter.
There are other types of noise, like Pink noise, and Blue noise. What makes them
different is just the way they manipulate the frequency distributions in the audio.

But let's at something a little bit more interesting and well known, Perlin noise.
The algorithm behind Perlin noise is actually quite elegant.

TODO: explain how the algorithm works in detail

```cpp
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

void generate_perlin_noise(
    std::mt19937& engine,
    std::uniform_int_distribution<int16_t>& dist,
    std::vector<int16_t>& samples) {
    float frequency = 440;
    float sampling_rate = 44100;
    for (size_t i = 0; i < samples.size(); i++) {
        float t = (i / sampling_rate) * frequency;
        samples[i] = (int16_t)(-32767.0f + perlin(t, 0) * 65535.0f);
    }
}
```

TODO: describe how it sounds like.

Perlin noise has limitations though. It doesn't scale well to higher dimensions, it has
noticable directional artifacts.
[Simplex noise](https://en.wikipedia.org/wiki/Simplex_noise) solves that while
also being easier to implement and more efficient. I don't fully understand
the math behind the algorith, but the wikipedia page on how to implement the algorithm
is pretty straight forwards.