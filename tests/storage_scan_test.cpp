#include "storage_scan.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace {

void touch(const std::string& path) {
    std::ofstream file(path);
    file << "test fixture - not game data";
}

} // namespace

int main() {
    char temporary_template[] = "/tmp/broodos-test-XXXXXX";
    char* temporary_root = mkdtemp(temporary_template);
    assert(temporary_root != nullptr);

    const std::string root(temporary_root);
    const std::string nested = root + "/USB/BroodOS";
    assert(mkdir((root + "/USB").c_str(), 0700) == 0);
    assert(mkdir(nested.c_str(), 0700) == 0);
    touch(nested + "/StarDat.mpq");
    touch(nested + "/BrooDat.mpq");
    touch(nested + "/Patch_rt.mpq");

    const broodos::ScanResult result = broodos::scan_for_game_data({root}, 4);
    assert(result.locations.size() == 1);
    assert(result.locations.front().complete());
    assert(!result.locations.front().patch_rt.empty());

    std::remove((nested + "/StarDat.mpq").c_str());
    std::remove((nested + "/BrooDat.mpq").c_str());
    std::remove((nested + "/Patch_rt.mpq").c_str());
    rmdir(nested.c_str());
    rmdir((root + "/USB").c_str());
    rmdir(root.c_str());
    return 0;
}

