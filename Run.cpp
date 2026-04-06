#include "App/App.hpp"

App app;

int main(const int argc, const char** argv)
{
    // This gets modified if we get some command line arguments (to start the benchmark from the terminal)
    // It is passed to app's Init method
    AppStartupModifiers mod{};

    // `./DiagramPche_DearImGui b x y` for the nodes benchmark
    // Where x is type of the benchmark, and y is the text editor mode.
    if (argc == 4 && strcmp(argv[1], "b") == 0) {
        constexpr auto benchmark_type_err_msg =
            "Second parameter (benchmark type) must be a number:\n\n\t0 - light\n\t1 - heavy\n\t2 - gradual\n\t3 - complete 3in1\n\n";

        try {
            mod.benchmark_type = std::stoi(argv[2]);
        }
        catch (const std::exception&) {
            std::cerr << benchmark_type_err_msg;
            return 1;
        }
        if (mod.benchmark_type < 0 || mod.benchmark_type > 3) {
            std::cerr << benchmark_type_err_msg;
            return 1;
        }

        if (strcmp(argv[3], "0") == 0) {
            mod.do_syntax_highlight = false;
        }
        else if (strcmp(argv[3], "2") == 0) {
            mod.do_skip_textedit = true;
        }

        mod.do_benchmark_nodes = true;
        mod.is_benchmark_run_from_terminal = true;
    }
    // `./DiagramPche_DearImGui w` for the widget benchmark
    else if (argc == 2 && strcmp(argv[1], "w") == 0) {
        mod.do_benchmark_widgets = true;
        mod.is_benchmark_run_from_terminal = true;
    }

    // Start the app
    if (app.Init(mod)) {
        app.Run();
    }

    return 0;
}
