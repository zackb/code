#include "FileUtils.h"

#include <filesystem>
#include <string>

#if __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

std::string getExecutablePath() {
#if __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::filesystem::canonical(path).parent_path().string();
    }
#endif
    return {};
}

std::string resolveAssetPath(const std::string& relativePath) {
    namespace fs = std::filesystem;

    // 1. Try raw relative path (e.g., "assets/map.csv")
    if (fs::exists(relativePath)) {
        return relativePath;
    }

    // 2. Try "assets/" + relativePath
    std::string assetRelative = "assets/" + relativePath;
    if (fs::exists(assetRelative)) {
        return assetRelative;
    }

#if __APPLE__
    // 3. Try locating Resources inside .app bundle
    std::string exePath = getExecutablePath();

    // Look for "Meconium.app/Contents/MacOS" and backtrack to "Resources"
    auto macosDir = fs::path(exePath);
    auto contentsDir = macosDir.parent_path();
    auto resourcesDir = contentsDir / "Resources";
    if (fs::exists(resourcesDir)) {
        auto fullPath = (resourcesDir / relativePath).string();
        if (fs::exists(fullPath)) {
            return fullPath;
        }

        // Try "assets/" prefix within Resources too
        fullPath = (resourcesDir / "assets" / relativePath).string();
        if (fs::exists(fullPath)) {
            return fullPath;
        }
    }
#endif

    // 4. Fallback: return unchanged and let caller handle error
    return relativePath;
}
