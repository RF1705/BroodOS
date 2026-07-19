#pragma once

#include <string>
#include <vector>

namespace broodos {

struct GameDataLocation {
    std::string directory;
    std::string star_dat;
    std::string broo_dat;
    std::string patch_rt;

    bool complete() const { return !star_dat.empty() && !broo_dat.empty(); }
};

struct ScanResult {
    std::vector<std::string> roots;
    std::vector<std::string> inaccessible_roots;
    std::vector<GameDataLocation> locations;
};

std::vector<std::string> default_scan_roots(const std::string& explicit_directory);
ScanResult scan_for_game_data(const std::vector<std::string>& roots, int maximum_depth = 5);

} // namespace broodos

