//
// Created by igor on 25/03/2021.
//

#ifndef TOMBEXCAVATOR_DIRECTORY_ADAPTOR_HH
#define TOMBEXCAVATOR_DIRECTORY_ADAPTOR_HH

#include <tomb-excavator/provider/vfs/directory.hh>
#include <tomb-excavator/games/common/vfs/data_loader.hh>

namespace games::common
{
    class directory_adaptor : public provider::vfs::directory
    {
    public:
        explicit directory_adaptor(std::shared_ptr<abstract_directory> ad);
    private:
        std::size_t entries() const override;
        std::string name(std::size_t entry_idx) const override;
        bool is_directory(std::size_t entry_idx) const override;
        provider::file_content_t open_file(std::size_t entry_idx) const override;
        provider::file_type_t file_type(std::size_t entry_idx) const override;
        std::unique_ptr<directory> load_directory(std::size_t entry_idx) const override;
    private:
        std::shared_ptr<abstract_directory> m_adapted;
    };
}

#endif //TOMBEXCAVATOR_DIRECTORY_ADAPTOR_HH
