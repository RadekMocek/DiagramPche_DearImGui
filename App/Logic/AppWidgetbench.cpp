#include <chrono>

#include "../../Dependency/RSS.hpp"

#include "../App.hpp"
#include "../Helper/CPU.hpp"

void App::HandleWidgetbench()
{
    // ReSharper disable once CppTooWideScopeInitStatement
    //constexpr auto WIDGETBENCH_STOP = 262144;
    constexpr auto WIDGETBENCH_STOP = 16;

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
            m_WB_log_data.duration.push_back(ChronoTrigger(m_WB_timestamp_window_queued).count());
            // LOG RAM
            constexpr auto MIBI = 1024.0 * 1024.0;
            m_WB_log_data.mem_mib.push_back(static_cast<double>(getCurrentRSS()) / MIBI);
            // LOG CPU
            m_WB_log_data.cpu_usage.push_back(CPUStats::GetCurrentValue());
            // --- --- --- --- --- --- --- --- --- --- --- ---
            // Report progress
            m_source = std::format("[node.\"{} {}\"]", m_WB_n_batches, m_WB_batch_iter);
            if (m_do_use_alt_editor) {
                // Don't use `OnMSourceChanged` as it's marking the document as dirty (no need for that)
                m_alt_editor.SetText(m_source);
            }
            // Prepare batch for the next iter
            m_WB_batch_iter++;
            if (m_WB_batch_iter > 10) {
                m_WB_batch_iter = 0;
                m_WB_n_batches *= 2;
            }
        }
        else {
            // Widgetbench keep-going condition
            if (m_WB_n_batches <= WIDGETBENCH_STOP) {
                // This is where the widgetbench starts
                // We'll set var to show the window next iter
                m_WB_timestamp_window_queued = std::chrono::steady_clock::now();
                m_WB_do_show_window = true;
                CPUStats::GetCurrentValue(); // Refresh CPU usage (?)
            }
            else {
                // This is where the widgetbench ends
                m_WB_is_running = false;
                // Filename
                const auto filename = std::format("./widgetbechres_DearImGui_{}_{}.csv", OS_ID, GetUNIXTimestamp());
                // Save
                if (WriteWidgetbenchResultsToCSV(filename.c_str(), m_WB_log_data)) {
                    std::cout << "Benchmark data written to '" << filename << "'.\n";
                }
                else {
                    std::cout << "Error writing benchmark data to file.";
                }
                // Let know
                m_source = "[node.\"Widget benchmark done\"]";
                if (m_do_use_alt_editor) {
                    // Don't use `OnMSourceChanged` as it's marking the document as dirty (no need for that)
                    m_alt_editor.SetText(m_source);
                }
            }
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
        // Refresh CPU usage (?)
        CPUStats::GetCurrentValue();
    }
}
