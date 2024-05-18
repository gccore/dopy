#include <filesystem>
#include <fstream>
#include <iostream>
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
auto constexpr kDestDir = "dopy_release";
auto constexpr kBinaryDir = "binary";
auto constexpr kManifestDir = "manifest";
} // namespace constants

struct Module final {
        using Vec = std::vector<Module>;

        std::string display_name;
        std::string path;
};

bool operator<(Module const& left, Module const& right)
{
        return left.path < right.path;
}

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

std::string GetTheName(std::filesystem::directory_entry const& item)
{
        return *std::prev(std::filesystem::path(item).end(), 1);
}

Module::Vec ManifestsList()
{
        ManifestInfos const manifest_infos = ManifestDirectories();
        std::filesystem::directory_iterator const aos_a{ manifest_infos.aos_a };
        std::filesystem::directory_iterator const aos_b{ manifest_infos.aos_b };
        Module::Vec result;

        result.reserve(constants::kMaxModule);
        for (auto const& dir : aos_a) {
                if (dir.is_directory()) {
                        Module module;
                        module.display_name = GetTheName(dir);
                        module.path = dir.path();
                        result.emplace_back(module);
                }
        }
        for (auto const& dir : aos_b) {
                if (dir.is_directory()) {
                        Module module;
                        module.display_name = GetTheName(dir);
                        module.path = dir.path();
                        result.emplace_back(module);
                }
        }

        return result;
}

Module::Vec BinariesList()
{
        using std::filesystem::directory_iterator;
        using std::filesystem::directory_entry;
        using std::filesystem::current_path;
        using std::filesystem::status;
        using std::filesystem::perms;

        directory_iterator const build_dir{ current_path() / "modules" };
        Module::Vec result;

        auto IsExecutable = +[](directory_entry const& other) -> bool {
                return other.is_regular_file() &&
                       (status(other.path()).permissions() & perms::owner_exec) != perms::none;
        };

        result.reserve(constants::kMaxModule);
        for (auto const& dir : build_dir) {
                if (dir.is_directory()) {
                        for (auto const& inner_dir : directory_iterator(dir)) {
                                if (IsExecutable(inner_dir)) {
                                        Module module;
                                        module.display_name = GetTheName(inner_dir);
                                        module.path = inner_dir.path();
                                        result.emplace_back(module);
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
        FileList(details::Module::Vec&& file_list);

        std::map<details::Module, bool> const& list() const;

    private:
        void configure(details::Module::Vec&& file_list);
        void generateView();

        std::map<details::Module, bool> list_;
};

FileList::FileList(details::Module::Vec&& file_list)
{
        configure(std::move(file_list));
        generateView();
}

std::map<details::Module, bool> const& FileList::list() const
{
        return list_;
}

void FileList::configure(details::Module::Vec&& file_list)
{
        for (details::Module file : file_list) {
                list_[std::move(file)] = false;
        }
}
void FileList::generateView()
{
        ftxui::Components components;

        for (auto& [key, value] : this->list_) {
                components.emplace_back(ftxui::Checkbox(key.display_name, &value));
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

void CopyFiles(FileList const& file_list, std::string const& dest)
{
        using std::filesystem::exists;
        using std::filesystem::create_directories;
        using std::filesystem::remove_all;
        using std::filesystem::copy_options;

        if (exists(dest)) {
                remove_all(dest);
        }
        create_directories(dest);

        std::ofstream log("log.txt", std::ios::app);

        for (auto const& [module, is_selected] : file_list.list()) {
                log << module.path << " - " << std::boolalpha << is_selected << std::endl;
                if (is_selected) {
                        std::filesystem::copy(module.path, dest,
                                              copy_options::recursive |
                                                      copy_options::overwrite_existing);
                }
        }
}

} // namespace cli

int main(void)
{
        int width = 30;
        int height = 20;

        ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

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
        ftxui::Component done_button = ftxui::Button("Done", [&] {
                try {
                        std::filesystem::path const dest =
                                std::filesystem::current_path() / details::constants::kDestDir;
                        cli::CopyFiles(
                                *dynamic_cast<cli::FileList*>(binaries_win_options.inner.get()),
                                dest / details::constants::kBinaryDir);
                        cli::CopyFiles(
                                *dynamic_cast<cli::FileList*>(manifests_win_options.inner.get()),
                                dest / details::constants::kManifestDir);
                } catch (std::exception& exception) {
                        std::ofstream file("log2.txt");
                        file << exception.what() << std::endl;
                }

                screen.ExitLoopClosure();
                screen.Exit();
        });

        ftxui::Component layout = ftxui::Container::Vertical({
                ftxui::Container::Horizontal({
                        manifests_window,
                        binaries_window,
                }),
                done_button,
        });

        auto component = Renderer(layout, [&] {
                return ftxui::vbox({ layout->Render() }) | ftxui::xflex |
                       ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 40) | ftxui::border;
        });

        screen.Loop(component);
}
