#pragma once

#include <array>
#include <chrono>
#include <fstream>

constexpr auto LOG_CAPACITY = 52;

struct BenchmarkLogResults
{
    std::array<long long, LOG_CAPACITY> timestamp{};
    std::array<int, LOG_CAPACITY> fps{};
    std::array<int, LOG_CAPACITY> n_nodes{};
    std::array<double, LOG_CAPACITY> mem_mib{};
    std::array<float, LOG_CAPACITY> cpu_usage{};
};

inline bool WriteBenchmarkResultsToCSV(const char* filename, const BenchmarkLogResults& res)
{
    if (std::ofstream file(filename); file.is_open()) {
        file << "timestamp,fps,n_nodes,mem_mib,cpu_usage\n";

        for (int i = 0; i < LOG_CAPACITY; i++) {
            file
                << res.timestamp[i]
                << "," << res.fps[i]
                << "," << res.n_nodes[i]
                << "," << res.mem_mib[i]
                << "," << res.cpu_usage[i]
                << "\n";
        }

        return true;
    }
    return false;
}

// === === === === === === === === === === === === === === === === === === === ===

template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto ChronoTrigger(std::chrono::time_point<clock_t, duration_t> const& time_start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - time_start);
}
