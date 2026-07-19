#include "storage_scan.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <dirent.h>
#include <set>
#include <sys/stat.h>
#include <unistd.h>

namespace broodos {
namespace {

std::string join_path(const std::string& left, const std::string& right) {
    if (left.empty() || left == "/") {
        return left + right;
    }
    return left + "/" + right;
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

bool is_directory(const std::string& path) {
    struct stat details {};
    return stat(path.c_str(), &details) == 0 && S_ISDIR(details.st_mode);
}

bool should_skip(const std::string& name) {
    const std::string lowered = lowercase(name);
    return name == "." || name == ".." || lowered == ".trash" ||
           lowered == ".trashes" || lowered == "system volume information" ||
           lowered == "lost+found";
}

void inspect_directory(const std::string& path,
                       int depth,
                       int maximum_depth,
                       std::vector<GameDataLocation>& locations,
                       std::set<std::string>& visited) {
    if (depth > maximum_depth || visited.count(path) != 0) {
        return;
    }
    visited.insert(path);

    DIR* directory = opendir(path.c_str());
    if (!directory) {
        return;
    }

    GameDataLocation candidate;
    candidate.directory = path;
    std::vector<std::string> child_directories;

    while (dirent* entry = readdir(directory)) {
        const std::string name(entry->d_name);
        if (should_skip(name)) {
            continue;
        }

        const std::string full_path = join_path(path, name);
        const std::string lowered = lowercase(name);
        if (lowered == "stardat.mpq") {
            candidate.star_dat = full_path;
        } else if (lowered == "broodat.mpq" || lowered == "brooddat.mpq") {
            candidate.broo_dat = full_path;
        } else if (lowered == "patch_rt.mpq") {
            candidate.patch_rt = full_path;
        }

        if (depth < maximum_depth && is_directory(full_path)) {
            child_directories.push_back(full_path);
        }
    }
    closedir(directory);

    if (candidate.complete()) {
        locations.push_back(candidate);
    }

    for (const std::string& child : child_directories) {
        inspect_directory(child, depth + 1, maximum_depth, locations, visited);
    }
}

void add_unique(std::vector<std::string>& roots, const std::string& path) {
    if (!path.empty() && std::find(roots.begin(), roots.end(), path) == roots.end()) {
        roots.push_back(path);
    }
}

} // namespace

std::vector<std::string> default_scan_roots(const std::string& explicit_directory) {
    std::vector<std::string> roots;
    add_unique(roots, explicit_directory);

    const char* environment_path = std::getenv("BROODOS_DATA_DIR");
    if (environment_path) {
        add_unique(roots, environment_path);
    }

    add_unique(roots, ".");
    add_unique(roots, "/media/internal/broodos");
    add_unique(roots, "/media/internal/BroodOS");
    add_unique(roots, "/tmp/usb");
    add_unique(roots, "/mnt/usb");
    return roots;
}

ScanResult scan_for_game_data(const std::vector<std::string>& roots, int maximum_depth) {
    ScanResult result;
    result.roots = roots;
    std::set<std::string> visited;

    for (const std::string& root : roots) {
        if (!is_directory(root) || access(root.c_str(), R_OK) != 0) {
            result.inaccessible_roots.push_back(root);
            continue;
        }
        inspect_directory(root, 0, maximum_depth, result.locations, visited);
    }
    return result;
}

} // namespace broodos

