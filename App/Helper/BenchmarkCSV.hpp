#pragma once

#include <chrono>
#include <fstream>
#include <vector>

struct BenchmarkLogResults
{
    std::vector<long long> timestamp{};
    std::vector<float> fps{};
    std::vector<int> n_nodes{};
    std::vector<double> mem_mib{};
    std::vector<float> cpu_usage{};
};

inline bool WriteBenchmarkResultsToCSV(const char* filename, const BenchmarkLogResults& res)
{
    if (std::ofstream file(filename); file.is_open()) {
        file << "timestamp,fps,n_nodes,mem_mib,cpu_usage\n";

        const auto log_capacity = res.timestamp.size();
        for (int i = 0; i < log_capacity; i++) {
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

// === === === === === === === === === === === === === === === === === === === === === === === ===

struct WidgetbenchLogResults
{
    std::vector<int> n_batches{};
    std::vector<int> batch_iter{};
    std::vector<long long> duration{};
    std::vector<double> mem_mib{};
    std::vector<float> cpu_usage{};
};

inline bool WriteWidgetbenchResultsToCSV(const char* filename, const WidgetbenchLogResults& res)
{
    if (std::ofstream file(filename); file.is_open()) {
        file << "n_batches,iter,duration,mem_mib,cpu_usage\n";

        const auto log_capacity = res.duration.size();
        for (int i = 0; i < log_capacity; i++) {
            file
                << res.n_batches[i]
                << "," << res.batch_iter[i]
                << "," << res.duration[i]
                << "," << res.mem_mib[i]
                << "," << res.cpu_usage[i]
                << "\n";
        }

        return true;
    }
    return false;
}

// === === === === === === === === === === === === === === === === === === === === === === === ===

template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto ChronoTrigger(std::chrono::time_point<clock_t, duration_t> const& time_start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - time_start);
}

inline long long GetUNIXTimestamp()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
