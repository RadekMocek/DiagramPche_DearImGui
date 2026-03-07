#pragma once

constexpr auto WELCOME_TOML = R"([variables]
offset = 14

[node.hello]
value = "Hello,"
color = "#ff99b880"
type = "ellipse"

[node.world]
value = "world!"
color = "#ffcfb380"
pivot = "top-left"
xy = ["hello", "bottom-right", "offset", "offset"]
type = "diamond"

[[path]]
start = ["hello", "bottom", 0, 0]
end = ["world", "bottom", 0, 0]
shift = 40
points = [["", "start", 0, "", "end", 0]]
color = [40, 40, 40, 255]

# ---------------

[node.testp]
xy = [250, 250]
value = "aaaaa\nbbbbbbbbbbbbb\nccccc\nddddd"
type = "text"

[node.teststh]
xy=[300, 0]
color_border = "#FF0000FF"
label_shift = [-50, -50]

[node.testm]
xy = [-250, -250]
value = "aaaaa\nbbbbb\nccccc\nddddd"
type = "text"

[node.testr]
color = "#FFFFFFC0"
pivot = "center"
xy = ["testp", "center", 0, 0]
value = "       "
z = 5
type = "rectangle"

[[path]]
start=["world", "right", 0 ,0]
end=["testp", "left", 0 ,0]
points=[
    ["teststh","bottom",0,"","start",0],
    ["hello","bottom",0,"","prev",200]
]
label=["huh?",0,0]

[[path]]
start=["testp", "right", 0,0]
ends=[
    ["teststh", "right", 0,0],
    ["teststh", "bottom", 20,0]
]
shift=100
label=["AAA\nBBB\nCCC\nDDD", 0,0]
)";
