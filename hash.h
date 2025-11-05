#pragma once

#include <string>
#include <sstream>
#include <random>
#include <execution>
#include <numeric>
#include <thread>
#include <vector>
#include <algorithm>
#include <iomanip>

std::string Hash(const std::string& input)
{
    // --- Parallel summation stage ---
    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t chunk_size = (input.size() + num_threads - 1) / num_threads;

    std::vector<uint64_t> partial_sums(num_threads, 0);

    std::for_each(
        std::execution::par_unseq,
        partial_sums.begin(),
        partial_sums.end(),
        [&](uint64_t& local_sum)
        {
            size_t thread_index = &local_sum - &partial_sums[0];
            size_t start = thread_index * chunk_size;
            size_t end = std::min(start + chunk_size, input.size());

            std::mt19937_64 prng(thread_index * 0x9e3779b97f4a7c15ULL + 123456789ULL);

            for (size_t i = start; i < end; ++i)
            {
                uint64_t mix = static_cast<uint64_t>(static_cast<unsigned char>(input[i])) * 0x9e3779b97f4a7c15ULL;
                mix ^= (i + thread_index * 0x85ebca6bUL);
                local_sum += prng() ^ mix;
            }
        });

    uint64_t sum = std::reduce(std::execution::par_unseq, partial_sums.begin(), partial_sums.end());

    // --- Hash postprocessing (same as your original logic) ---
    int output[64];
    std::fill_n(output, 64, 15);

    std::mt19937 gen(static_cast<unsigned int>(sum));
    std::uniform_int_distribution<> distr(0, 15);

    std::stringstream stream;
    stream << std::hex << std::setfill('0');

    for (int num : output)
        stream << std::hex << (num - distr(gen));

    return stream.str();
}
