#pragma once

#ifdef _WIN32
#include "pdh.h"
#else
#include "../../Dependency/linux-system-usage.hpp"
#endif

constexpr auto CPU_MEASURE_INTERVAL_MS = 500.0f;
constexpr auto CPU_MEASURE_INTERVAL_S = CPU_MEASURE_INTERVAL_MS / 1000.0f;

class CPUStats
{
private:
#ifdef _WIN32
    inline static PDH_HQUERY cpu_query;
    inline static PDH_HCOUNTER cpu_total;
#else
    inline static get_system_usage_linux::CPU_stats cpu_data;
#endif

public:
    static void Init()
    {
#ifdef _WIN32
        // "Creates a new query that is used to manage the collection of performance data."
        // Params: [where to collect data, null means realtime] [tag to retrieve data later, not needed] [the handle we'll use to monitor the performance]
        PdhOpenQueryW(nullptr, 0, &cpu_query);

        // "Adds the specified language-neutral counter to the query."
        // Params: [handle] [counter path] [tag to retrieve data later, not needed] [counter handle]
        PdhAddEnglishCounterW(cpu_query, L"\\Processor(_Total)\\% Processor Time", 0, &cpu_total);

        // "Collects the current raw data value for all counters in the specified query and updates the status code of each counter."
        PdhCollectQueryData(cpu_query);
#else
        cpu_data = get_system_usage_linux::read_cpu_data();
#endif
    }

    static float GetCurrentValue()
    {
#ifdef _WIN32
        PDH_FMT_COUNTERVALUE counter_value;
        PdhCollectQueryData(cpu_query);
        PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, nullptr, &counter_value);
        return static_cast<float>(counter_value.doubleValue);
#else
        const get_system_usage_linux::CPU_stats new_cpu_data = get_system_usage_linux::read_cpu_data();
        const auto usage = get_system_usage_linux::get_cpu_usage(cpu_data, new_cpu_data);
        cpu_data = new_cpu_data;
        return 100.0f * usage;
#endif
    }
};
