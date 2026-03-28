#include <chrono>

#include "../../Dependency/RSS.hpp"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/CPU.hpp"

void App::HandleWidgetbench()
{
    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr auto DURATION_THRESHOLD_MS = 15000;

    // RUNNING
    if (m_WB_is_running) {
        if (m_WB_do_show_window) {
            // Window has been shown, do not show it in the next iteration, in which will set the var to show it again
            m_WB_do_show_window = false;
            // --- --- --- --- --- --- --- --- --- --- --- ---
            // LOG N "ROWS"
            m_WB_log_data.n_batches.push_back(m_WB_n_batches);
            m_WB_log_data.batch_iter.push_back(m_WB_batch_iter);
            // LOG DURATION
            const auto duration_ms = ChronoTrigger(m_WB_timestamp_window_queued).count();
            m_WB_log_data.duration.push_back(duration_ms);
            // LOG RAM
            constexpr auto MIBI = 1024.0 * 1024.0;
            m_WB_log_data.mem_mib.push_back(static_cast<double>(getCurrentRSS()) / MIBI);
            // LOG CPU
            m_WB_log_data.cpu_usage.push_back(m_CPU_usage);
            // --- --- --- --- --- --- --- --- --- --- --- ---
            // Report progress
            m_source = std::format("[node.\"{} {}\"]", m_WB_n_batches, m_WB_batch_iter);
            OnMSourceChanged(false);
            // Prepare batch for the next iter
            m_WB_batch_iter++;
            if (m_WB_batch_iter > 9) {
                m_WB_batch_iter = 0;
                m_WB_n_batches *= 2;
            }
            // STOP CONDITION
            if (duration_ms > DURATION_THRESHOLD_MS) {
                // This is where the widgetbench ends
                m_WB_is_running = false;
                // Filename
                const auto filename = std::format("./widgetbechres_Qt_{}_{}.csv", OS_ID, GetUNIXTimestamp());
                // Save
                if (WriteWidgetbenchResultsToCSV(filename.c_str(), m_WB_log_data)) {
                    std::cout << "Benchmark data written to '" << filename << "'.\n";
                }
                else {
                    std::cout << "Error writing benchmark data to file.";
                }
                // Let know
                m_source = "[node.\"Widget benchmark done\"]";
                OnMSourceChanged(false);
                // Exit actually?
                // ReSharper disable once CppRedundantBooleanExpressionArgument
                if (EXIT_AFTER_BENCHMARK_FROM_TERMINAL && m_app_startup_modifiers.is_benchmark_run_from_terminal) {
                    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
                }
            }
        }
        else {
            // This is where the widgetbench starts
            // We'll set var to show the window next iter
            m_WB_timestamp_window_queued = std::chrono::steady_clock::now();
            m_WB_do_show_window = true;
        }
    }
    // INIT
    if (m_app_startup_modifiers.do_benchmark_widgets) {
        m_app_startup_modifiers.do_benchmark_widgets = false;
        // (Re)set benchmark data
        m_WB_do_show_window = false;
        m_WB_n_batches = 1;
        m_WB_batch_iter = 1;
        m_WB_is_running = true;
        // Maximize the window
        glfwMaximizeWindow(m_window);
    }
}
