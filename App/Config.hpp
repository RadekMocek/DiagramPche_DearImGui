#pragma once

#include "imgui.h"

// Benchmarks
constexpr auto EXIT_AFTER_BENCHMARK_FROM_TERMINAL = true;

// Used in toolbar and settings window to set boundaries for the InputInt
constexpr auto FONT_SIZE_SOURCE_MIN = 8;
constexpr auto FONT_SIZE_SOURCE_MAX = 40;

// Used for parsing error message, error modal message, SVG export overwrite warning message
constexpr auto COLOR_ERROR = IM_COL32(211, 1, 2, 255);

// Used in body and toolbar
constexpr auto SEPARATOR_WIDTH = 4.0f;

// Arrow tips config used for drawing and SVG rendering
constexpr auto TIP_ARROW_LENGTH = 12;
constexpr auto TIP_ARROW_SPAN = 4;
