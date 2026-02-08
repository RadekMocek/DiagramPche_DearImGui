#pragma once

// "DL" == Draw List

constexpr auto N_DL_USER_CHANNELS = 10;
constexpr auto N_DL_REAL_CHANNELS = 2 * N_DL_USER_CHANNELS;
constexpr auto DL_USER_CHANNEL_DEFAULT_NODE = 4;
constexpr auto DL_USER_CHANNEL_DEFAULT_PATH = 5;

constexpr int DLUserChannelToRealChannel(const int input, const bool is_node)
{
    return (is_node) ? 2 * input : 2 * input + 1;
}
