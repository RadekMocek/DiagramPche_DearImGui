#include "../../Dependency/RSS.hpp"

#include "../App.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/GUILayout.hpp"

// (In this benchmark, nodes are being added to the canvas. (They are added as pairs connected by arrow.))
// After this passes, add new batch of nodes.
constexpr auto TIME_INTERVAL = 0.3f;
// How many nodes to add in a batch?
constexpr auto N_NODES_IN_INTERVAL = 35;
// Add this to each node x coordinate.
constexpr auto X_COR_ADDITION = 12;
// How many Z layers we use? Each node has Z one greater than the previous, moduled by this.
constexpr auto Z_MODULO = N_DL_USER_CHANNELS;
// How many nodes on a row we want?
constexpr auto MAX_NODES_ON_ROW = 220;
// When we reach `MAX_NODES_ON_ROW`, we go on a new row, this is the offset of the new row.
constexpr auto Y_COR_ADDITION = 100;
// How many rows do we want? When we have this much of rows, benchamrk ends.
constexpr auto MAX_ROWS = 21;
// Used for the ending condition.
constexpr auto MAX_Y_COR = Y_COR_ADDITION * MAX_ROWS;
// (While benchmarking, we also scroll and zoom, so we have some movement.)
// Amount of scrolling right after each node batch added.
constexpr auto AUTO_SCROLL_STEP_X = 10;
// When to wrap to the beggining with the scrolling.
constexpr auto AUTO_SCROLL_MODULO_X = 600;
// How many zoom levels we iterate, this corresponds to the slider and MW behavior.
constexpr auto ZOOM_LEVEL_MODULO = 6;

void App::BenchmarkStart()
{
    // This is called when user presses the 'Start benchmark' button.
    HandleRegularNew();
    ResetCanvasScrollingAndZoom();
    m_is_benchmark_running = true;
    m_is_benchmark_first_iter = true;
    m_body_split_ratio = 0.28f;

    // Reserve string space
    m_source.reserve(1000000);
}

void App::BenchmarkUpdate()
{
    // Helper variables used during the benchmark.
    static float time_counter;
    static int node_counter_total_pairs;
    static int node_counter_row_pairs;
    static int x_cor;
    static int y_cor;
    static unsigned char color_r;
    static unsigned char color_g;
    static unsigned char color_b;
    static int zoom_level;

    if (m_is_benchmark_running) {
        if (m_is_benchmark_first_iter) {
            m_is_benchmark_first_iter = false;
            // Initialize stats
            m_bench_stats_total_nodes = 0;
            // Initialize helper variables.
            time_counter = 0;
            node_counter_total_pairs = 0;
            node_counter_row_pairs = 0;
            x_cor = 0;
            y_cor = 0;
            color_r = 255;
            color_g = 255;
            color_b = 255;
            zoom_level = 0;
        }
        // Get delta time from ImGui.
        const ImGuiIO& io = ImGui::GetIO();
        time_counter += io.DeltaTime;

        if (time_counter > TIME_INTERVAL) {
            time_counter -= TIME_INTERVAL;

            // Zoom frenzy
            zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO; // 0,1,2,3,4,5
            ChangeCanvasFontSizeAndZoom(6 + 4 * zoom_level);

            // Add a new batch of nodes.
            for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
                const auto z = node_counter_row_pairs % Z_MODULO;
                m_source += std::format(
                    "[node.\"A{}\"]\nxy=[{},{}]\nz={}\ncolor=[{},{},{},128]\n"
                    "[node.\"B{}\"]\nxy=[\"A{}\",\"bottom-right\",10,10]\nz={}\ntype=\"ellipse\"\n"
                    "[[path]]\nstart=[\"A{}\",\"left\",0,0]\nend=[\"B{}\",\"right\",0,0]\n",
                    node_counter_total_pairs, x_cor, y_cor, z, color_r, color_g, color_b,
                    node_counter_total_pairs, node_counter_total_pairs, z,
                    node_counter_total_pairs, node_counter_total_pairs
                );
                node_counter_total_pairs++;
                node_counter_row_pairs++;
                x_cor += X_COR_ADDITION;
                // Modify the color for next node; accepts number from 0 to 5 as last param to shuffle things up, we can use zoom level
                BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
            }

            // Auto scrolling
            m_scrolling.x -= AUTO_SCROLL_STEP_X;
            if (m_scrolling.x < -AUTO_SCROLL_MODULO_X) {
                m_scrolling.x = 0;
            }

            // Don't use `OnMSourceChanged` as it's marking the document as dirty (no need for that).
            if (m_do_use_alt_editor) m_alt_editor.SetText(m_source);

            // Jump to new row if needed
            if (node_counter_row_pairs > MAX_NODES_ON_ROW) {
                node_counter_row_pairs = 0;
                x_cor = 0;
                y_cor += Y_COR_ADDITION;
            }

            // End the benchmark if needed
            if (y_cor > MAX_Y_COR) {
                std::cout << "Benchmark done " << m_source.size() << "\n";
                m_is_benchmark_running = false;
            }

            // Stats
            m_bench_stats_total_nodes += 2 * N_NODES_IN_INTERVAL;
            BenchmarkStatsUpdate();

            //TODO log to CSV?
            //if (zoom_level == 1) {/*log stats*/}
        }
    }
}

void App::BenchmarkGUIUpdate()
{
    const ImGuiIO& io = ImGui::GetIO();

    ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(0.0f, 0.0f), "Benchmark is running...");

    ImGui::Text("      GL renderer: %s", gl_info_renderer);
    ImGui::Text("Average framerate: %.1f FPS", io.Framerate);
    ImGui::Text("Total nodes drawn: %i", m_bench_stats_total_nodes);
    ImGui::Text(" Working set size: %.1f MiB", m_bench_stats_mem_usage_mib);

    ImGui::Separator();
    ImGui::Dummy(TINY_SKIP);
    if (ImGui::Button("Stop")) {
        m_is_benchmark_running = false;
    }
}

void App::BenchmarkStatsUpdate()
{
    constexpr auto MIBI = 1024.0f * 1024.0f;
    m_bench_stats_mem_usage_mib = getCurrentRSS() / MIBI;
}
