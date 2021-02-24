//
// Created by igor on 22/02/2021.
//

#include <tomb-excavator/provider/vfs/file_system.hh>

class ccfs : public provider::vfs::file_system
{
public:
    ccfs();
private:
    ~ccfs() override;

    std::string name() const override;
    bool accept(const provider::physfs::directory& dir) const override;
    std::unique_ptr<provider::vfs::directory> root(const provider::physfs::directory& dir) const override;
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
    return nullptr;
}
// ----------------------------------------------------------------------------------------
REGISTER_PROVIDER_FS(ccfs)
