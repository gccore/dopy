#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

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

namespace cli
{
class FileList final : public ftxui::ComponentBase {
    public:
        FileList(std::vector<std::string>&& file_list);

    private:
        void configure(std::vector<std::string>&& file_list);
        void generateView();

        std::map<std::string, bool> list_;
};

FileList::FileList(std::vector<std::string>&& file_list)
{
        configure(std::move(file_list));
        generateView();
}

void FileList::configure(std::vector<std::string>&& file_list)
{
        for (std::string file : file_list) {
                list_[std::move(file)] = false;
        }
}
void FileList::generateView()
{
        ftxui::Components components;

        for (auto& [key, value] : this->list_) {
                std::string const filename = *std::prev(std::filesystem::path(key).end(), 1);
                components.emplace_back(ftxui::Checkbox(filename, &value));
        }

        this->ComponentBase::Add(ftxui::Container::Vertical(std::move(components)));
}

ftxui::Component ManifestsWindow()
{
        return ftxui::Make<FileList>(details::ManifestsList());
}
ftxui::Component BinariesWindow()
{
        return ftxui::Make<FileList>(details::BinariesList());
}

} // namespace cli

int main(void)
{
        int width = 30;
        int height = 65;

        ftxui::WindowOptions binaries_win_options;
        binaries_win_options.inner = cli::BinariesWindow();
        binaries_win_options.title = "Binaries List";
        binaries_win_options.width = &width;
        binaries_win_options.height = &height;

        ftxui::WindowOptions manifests_win_options;
        manifests_win_options.inner = cli::ManifestsWindow();
        manifests_win_options.title = "Manifests List";
        manifests_win_options.width = &width;
        manifests_win_options.height = &height;

        ftxui::Component manifests_window = ftxui::Window(manifests_win_options);
        ftxui::Component binaries_window = ftxui::Window(binaries_win_options);

        ftxui::Component layout = ftxui::Container::Horizontal({
                manifests_window,
                binaries_window,
        });
        ftxui::Component component = ftxui::Renderer(layout, [&] {
                return ftxui::hbox({
                               manifests_window->Render(),
                               binaries_window->Render(),
                       }) |
                       ftxui::xflex | ftxui::yflex | size(ftxui::WIDTH, ftxui::GREATER_THAN, 40) |
                       ftxui::border;
        });

        ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
        screen.Loop(component);
}
