//
// Created by igor on 22/02/2021.
//

#include <vector>

#include <tomb-excavator/provider/vfs/file_system.hh>
#include <tomb-excavator/provider/physfs/data_loader.hh>

#include "sprite_loader.hh"

class cc_directory : public provider::vfs::directory
{
public:
    explicit cc_directory(const provider::physfs::directory& dir)
    {
        m_loaders.emplace_back(new sprite_loader("sprites", "cc1.gfx", dir));
    }

private:
    [[nodiscard]] std::size_t entries() const override
    {
        return m_loaders.size();
    }

    [[nodiscard]] std::string name(std::size_t entry_idx) const override
    {
        return m_loaders[entry_idx]->name();
    }

    [[nodiscard]] bool is_directory([[maybe_unused]] std::size_t entry_idx) const override
    {
        return false;
    }

    [[nodiscard]] std::unique_ptr<directory> load_directory([[maybe_unused]] std::size_t entry_idx) const override
    {
        return nullptr;
    }

    [[nodiscard]] provider::file_type_t open_file(std::size_t entry_idx) const override
    {
        return m_loaders[entry_idx]->load();
    }
private:
    std::vector<std::unique_ptr<provider::physfs::data_loader>> m_loaders;
};
// =========================================================================================
class ccfs : public provider::vfs::file_system
{
public:
    ccfs();
private:
    ~ccfs() override;

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
    [[nodiscard]] std::unique_ptr<provider::vfs::directory> root(const provider::physfs::directory& dir) const override;
};

ccfs::ccfs() = default;
// ----------------------------------------------------------------------------------------
ccfs::~ccfs() = default;
// ----------------------------------------------------------------------------------------
std::string ccfs::name() const
{
    return "Crystal Caves";
}
// ----------------------------------------------------------------------------------------
bool ccfs::accept(const provider::physfs::directory& dir) const
{
    return dir.contains("cc1.gfx");
}
// ----------------------------------------------------------------------------------------
std::unique_ptr<provider::vfs::directory> ccfs::root(const provider::physfs::directory& dir) const
{
    return std::make_unique<cc_directory>(dir);
}
// ----------------------------------------------------------------------------------------
REGISTER_PROVIDER_FS(ccfs)
