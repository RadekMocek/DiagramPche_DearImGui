#pragma once

constexpr auto WELCOME_TOML = R"([variables]
node_offset = 14
path_offset = 40

[node.hello]
value = "Hello,"
color = "#ff99b880"
type = "ellipse"

[node.world]
value = "world!"
color = "#ffcfb380"
pivot = "top-left"
xy = ["hello", "bottom-right", "node_offset", "node_offset"]
type = "diamond"

[[path]]
start = ["hello", "bottom", 0, 0]
end = ["world", "bottom", 0, 0]
shift = [0, "path_offset"]
points = [["", "start", 0, "", "end", 0]]
color = [40, 40, 40, 255]
)";
