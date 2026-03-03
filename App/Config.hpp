#pragma once

#include "imgui.h"

constexpr auto FONT_SIZE_DEFAULT = 20.0f;
constexpr auto FONT_SIZE_MAIN_MENU_BAR = 18.0f;

constexpr auto BOTTOM_BAR_HEIGHT = FONT_SIZE_DEFAULT;

constexpr auto COLOR_ERROR = IM_COL32(211, 1, 2, 255);
constexpr auto COLOR_ERROR_HIGHLIGHT = IM_COL32(211, 1, 2, 80);

// Canvas
constexpr ImVec2 SCROLLING_DEFAULT = {5.0f, 5.0f};

constexpr float GRID_STEP_BASE = 100.0f;
constexpr auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);

constexpr int CANVAS_FONT_SIZE_BASE = 18;
constexpr int CANVAS_FONT_SIZE_STEP = 4;
constexpr int CANVAS_FONT_SIZE_MIN = 6;
constexpr int CANVAS_FONT_SIZE_MAX = 30;

constexpr float NODE_BORDER_OFFSET_BASE = 18.0f;

constexpr int TIP_ARROW_LENGTH = 12;
constexpr int TIP_ARROW_SPAN = 4;

constexpr auto WELCOME_TOML = R"([variables]
offset = 14

[node.hello]
value = "Hello,"
color = "#ff99b880"

[node.world]
value = "world!"
color = "#ffcfb380"
pivot = "top-left"
xy = ["hello", "bottom-right", "offset", "offset"]

[[path]]
start = ["hello", "bottom", 0, 0]
end = ["world", "bottom", 0, 0]
shift = 40
points = [["", "start", 0, "", "end", 0]]
color = [40, 40, 40, 255]

# Testing section

[node.test_node]
xy = [300, 300]
color = [140, 140, 140, 255]

# ---------------
)";
