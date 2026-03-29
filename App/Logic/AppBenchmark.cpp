#include "../../Dependency/RSS.hpp"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/CPU.hpp"
#include "../Helper/GUILayout.hpp"

// (In benchmark type GRADUAL, nodes are being added to the canvas (they are added as pairs connected by arrow))
// (While benchmarking, we also scroll and zoom, so we have some movement)
// (Benchmarks type LIGHT and HEAVY have prepared TOML and just do scroll and zoom above them)
// What percentage of the window's width will the text editor occupy during the benchmark
constexpr auto TEXTEDIT_WIDTH_RATIO = 0.28f;
// After this passes, add new batch of nodes
constexpr auto TIME_INTERVAL = 0.3f; // s
// How many nodes to add in a batch?
constexpr auto N_NODES_IN_INTERVAL = 35;
// Add this to each node x coordinate
constexpr auto X_COR_ADDITION = 12;
// How many Z layers we use? Each node has Z one greater than the previous, moduled by this
constexpr auto Z_MODULO = N_DL_USER_CHANNELS;
// How many nodes on a row we want?
constexpr auto MAX_NODES_ON_ROW = 220;
// When we reach `MAX_NODES_ON_ROW`, we go on a new row, this is the offset of the new row
constexpr auto Y_COR_ADDITION = 100;
// How many rows do we want? When we have this much of rows, benchmark ends
constexpr auto MAX_ROWS = 26;
// Used for the ending condition
constexpr auto MAX_Y_COR = Y_COR_ADDITION * MAX_ROWS;
// (While benchmarking, we also scroll and zoom, so we have some movement)
// Amount of scrolling right after each node batch added
constexpr auto AUTO_SCROLL_STEP_X = 10;
// When to wrap to the beggining with the scrolling
constexpr auto AUTO_SCROLL_MODULO_X = 600;
// How many zoom levels we iterate, this corresponds to the slider and MW behavior
constexpr auto ZOOM_LEVEL_MODULO = 6;
// Precalculated
constexpr auto BENCHMARK_LIGHT_N_NODES = 12;
constexpr auto BENCHMARK_HEAVY_N_NODES = 13230;

// This is called when user presses the 'Start benchmark' button
void App::BenchmarkStart(const BenchmarkType type)
{
    // Update state
    m_is_benchmark_running = true;
    m_benchmark_type = type;
    m_is_benchmark_first_iter = true;

    // Reset the view
    ResetCanvasScrollingAndZoom();

    // Change the ratio between textedit and canvas to make canvas bigger (more things to see)
    m_body_split_ratio = TEXTEDIT_WIDTH_RATIO;

    // Reserve string space
    if (type == BENCHMARK_GRADUAL || type == BENCHMARK_COMPLETE) {
        m_source.reserve(1224747);
    }

    // Maximize the window
    glfwMaximizeWindow(m_window);

    // Prepare the source
    if (type == BENCHMARK_LIGHT) {
        HandleOpenExample(BENCHMARK_LIGHT_PATH);
        m_bench_stats_total_nodes = BENCHMARK_LIGHT_N_NODES;
    }
    else if (type == BENCHMARK_HEAVY) {
        HandleOpenExample(BENCHMARK_HEAVY_PATH);
        m_bench_stats_total_nodes = BENCHMARK_HEAVY_N_NODES;
    }
    else if (type == BENCHMARK_GRADUAL) {
        HandleRegularNew();
        m_bench_stats_total_nodes = 0;
    }
    else if (type == BENCHMARK_COMPLETE) {
        HandleRegularNew();
        m_source = "[node.\"Hang onto yer helmet!\\nThe complete benchmark has started...\"]\n";
        if (m_do_use_alt_editor) m_alt_editor.SetText(m_source);
        m_bench_stats_total_nodes = 0;
    }
}

void App::BenchmarkUpdate()
{
    // Helper variables used during the benchmark
    static float time_counter;
    static int gradual_node_id;
    static int node_counter_row_pairs;
    static int x_cor;
    static int y_cor;
    static unsigned char color_r;
    static unsigned char color_g;
    static unsigned char color_b;
    static int zoom_level;
    static BenchmarkLogResults log_data;
    static std::chrono::time_point<std::chrono::steady_clock> time_start;
    static int complete_bench_phase_n;

    if (m_is_benchmark_running) {
        if (m_is_benchmark_first_iter) {
            m_is_benchmark_first_iter = false;
            // Initialize helper variables
            time_counter = 0;
            gradual_node_id = 0;
            node_counter_row_pairs = 0;
            x_cor = 0;
            y_cor = 0;
            color_r = 255;
            color_g = 255;
            color_b = 255;
            zoom_level = 0;
            log_data = {};
            time_start = std::chrono::steady_clock::now();
            complete_bench_phase_n = 1; // Basically: 1 – Light, 2 – Gradual, 3 – Heavy
        }
        // Get delta time from Dear ImGui
        const ImGuiIO& io = ImGui::GetIO();
        time_counter += io.DeltaTime;

        // Do the next batch only when certain amount of time has passed
        if (time_counter > TIME_INTERVAL) {
            time_counter -= TIME_INTERVAL;

            // Zoom frenzy
            zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO; // 0,1,2,3,4,5
            ChangeCanvasFontSizeAndZoomFromSliderValue(zoom_level);

            const auto is_gradual =
                m_benchmark_type == BENCHMARK_GRADUAL
                || (m_benchmark_type == BENCHMARK_COMPLETE && complete_bench_phase_n == 2);

            // Add a new batch of nodes
            for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
                if (is_gradual) {
                    const auto z = node_counter_row_pairs % Z_MODULO;
                    m_source += std::format(
                        "[node.\"A{}\"]\nxy=[{},{}]\nz={}\ncolor=[{},{},{},128]\n"
                        "[node.\"B{}\"]\nxy=[\"A{}\",\"bottom-right\",10,10]\nz={}\ntype=\"ellipse\"\n"
                        "[[path]]\nstart=[\"A{}\",\"left\",0,0]\nend=[\"B{}\",\"right\",0,0]\n",
                        gradual_node_id, x_cor, y_cor, z, color_r, color_g, color_b,
                        gradual_node_id, gradual_node_id, z,
                        gradual_node_id, gradual_node_id
                    );
                    gradual_node_id++;
                }
                node_counter_row_pairs++;
                x_cor += X_COR_ADDITION;
                // Modify the color for next node; accepts number from 0 to 5 as last param to shuffle things up, we can use zoom level
                BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
            }

            if (m_do_use_alt_editor && is_gradual) {
                // Don't use `OnMSourceChanged` as it's marking the document as dirty (no need for that)
                m_alt_editor.SetText(m_source);
            }

            // Auto scrolling
            m_scrolling.x -= AUTO_SCROLL_STEP_X;
            if (m_scrolling.x < -AUTO_SCROLL_MODULO_X) {
                m_scrolling.x = 0;
            }

            // Jump to new row if needed
            if (node_counter_row_pairs > MAX_NODES_ON_ROW) {
                node_counter_row_pairs = 0;
                x_cor = 0;
                y_cor += Y_COR_ADDITION;
            }

            // Stats
            if (is_gradual) {
                m_bench_stats_total_nodes += 2 * N_NODES_IN_INTERVAL;
            }

            if (zoom_level % 3 == 1) {
                constexpr auto MIBI = 1024.0 * 1024.0;
                m_bench_stats_mem_usage_mib = static_cast<double>(getCurrentRSS()) / MIBI;
                // LOG
                log_data.timestamp.push_back(ChronoTrigger(time_start).count());
                log_data.fps.push_back(io.Framerate);
                log_data.n_nodes.push_back(m_bench_stats_total_nodes);
                log_data.mem_mib.push_back(m_bench_stats_mem_usage_mib);
                log_data.cpu_usage.push_back(m_CPU_usage);
            }

            // End the benchmark check
            if (y_cor > MAX_Y_COR) {
                if (m_benchmark_type == BENCHMARK_COMPLETE && complete_bench_phase_n < 3) {
                    complete_bench_phase_n++;
                    x_cor = 0;
                    y_cor = 0;
                }
                else {
                    m_is_benchmark_running = false;
                    const auto bench_id = std::format("b{}", static_cast<int>(m_benchmark_type));

                    auto bench_info = (m_do_use_alt_editor) ? "shon" : "shoff";
                    if (m_do_skip_textedit) {
                        bench_info = "txoff";
                    }

                    const auto filename = std::format("./bnchres_DearImGui_{}_{}_{}_{}.csv",
                                                      OS_ID, bench_id, bench_info, GetUNIXTimestamp());
                    if (WriteBenchmarkResultsToCSV(filename.c_str(), log_data)) {
                        std::cout << "Benchmark data written to '" << filename << "'.\n";
                    }
                    else {
                        std::cout << "Error writing benchmark data to file.";
                    }

                    // ReSharper disable once CppRedundantBooleanExpressionArgument
                    if (EXIT_AFTER_BENCHMARK_FROM_TERMINAL && m_app_startup_modifiers.is_benchmark_run_from_terminal) {
                        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
                    }

                    /*
                    std::cout << m_bench_stats_total_nodes << "\n";
                    std::cout << m_source.size() << "\n";
                    //*/
                }
            }
        }
    }
}

void App::BenchmarkGUIUpdate()
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(0.0f, 0.0f), "Benchmark is running...");
    ImGui::Text("(GL renderer: %s)", gl_info_renderer);
    ImGui::Dummy(TINY_SKIP);
    ImGui::Separator();

    ImGui::Text("    App framerate: %.1f FPS", io.Framerate);
    ImGui::Text("Total nodes drawn: %i", m_bench_stats_total_nodes);
    ImGui::Text(" Working set size: %.1f MiB", m_bench_stats_mem_usage_mib);
    ImGui::Text(" System CPU usage: %.1f %%", m_CPU_usage);

    ImGui::Separator();
    ImGui::Dummy(TINY_SKIP);
    if (ImGui::Button("Stop")) {
        m_is_benchmark_running = false;
        std::cout << "Benchmark stopped.\n";
    }
}
