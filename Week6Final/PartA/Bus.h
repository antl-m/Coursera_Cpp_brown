#pragma once

#include <unordered_map>
#include <vector>
#include <string_view>

using Route = std::vector<std::string_view>;

using Buses = std::unordered_map<int, Route>;
