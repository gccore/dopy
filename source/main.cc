#include <array>
#include <cstdlib>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace details
{
namespace constants
{
auto constexpr kMaxModule = 20;
} // namespace constants

struct Module final {
        using Vec = std::vector<Module>;

        std::string name;
        std::string binary;
        std::string manifest;
};

struct ManifestInfos final {
        std::string aos_a;
        std::string aos_b;
};

ManifestInfos ManifestDirectories(void)
{
        auto constexpr kManifestDirName = "outputcfg";
        auto constexpr kAosADirName = "HostMachine";
        auto constexpr kAosBDirName = "Dmini3Machine";

        std::filesystem::path const cwd = std::filesystem::current_path();
        std::string const project_name = *std::prev(cwd.end(), 2);
        std::string const outputcfg =
                cwd.string() + "/../../../" + project_name + "/" + kManifestDirName;
        std::string aos_a = outputcfg + "/" + kAosADirName;
        std::string aos_b = outputcfg + "/" + kAosBDirName;

        return ManifestInfos{ std::move(aos_a), std::move(aos_b) };
}

std::vector<std::string> ManifestsList()
{
        ManifestInfos const manifest_infos = ManifestDirectories();
        std::filesystem::directory_iterator const aos_a{ manifest_infos.aos_a };
        std::filesystem::directory_iterator const aos_b{ manifest_infos.aos_b };
        std::vector<std::string> result;

        result.reserve(constants::kMaxModule);
        for (auto const& dir : aos_a) {
                if (dir.is_directory()) {
                        result.emplace_back(dir.path());
                }
        }
        for (auto const& dir : aos_b) {
                if (dir.is_directory()) {
                        result.emplace_back(dir.path());
                }
        }

        return result;
}

std::vector<std::string> BinariesList()
{
        using std::filesystem::directory_iterator;
        using std::filesystem::directory_entry;
        using std::filesystem::current_path;
        using std::filesystem::status;
        using std::filesystem::perms;

        directory_iterator const build_dir{ current_path() / "modules" };
        std::vector<std::string> result;

        auto IsExecutable = +[](directory_entry const& other) -> bool {
                return other.is_regular_file() &&
                       (status(other.path()).permissions() & perms::owner_exec) != perms::none;
        };

        result.reserve(constants::kMaxModule);
        for (auto const& dir : build_dir) {
                if (dir.is_directory()) {
                        for (auto const& inner_dir : directory_iterator(dir)) {
                                if (IsExecutable(inner_dir)) {
                                        result.emplace_back(inner_dir.path());
                                }
                        }
                }
        }

        return result;
}

} // namespace details

int main(void)
{
        std::vector<std::string> const manifests = details::ManifestsList();
        std::vector<std::string> const binaries = details::BinariesList();
}
