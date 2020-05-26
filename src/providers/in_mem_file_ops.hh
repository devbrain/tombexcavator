//
// Created by igor on 26/05/2020.
//

#ifndef TOMBEXCAVATOR_IN_MEM_FILE_OPS_HH
#define TOMBEXCAVATOR_IN_MEM_FILE_OPS_HH

#include <providers/provider_api.h>
#include <archive/io.hh>
#include <vfs/api/vfs_module.h>

namespace provider
{
    class PROVIDER_API in_mem_file_ops
    {
    public:
        in_mem_file_ops(std::vector<std::byte> memory);

        in_mem_file_ops(in_mem_file_ops&& other);

        ssize_t do_read  (void* buff, size_t len);
        bool seek (uint64_t pos, enum whence_type whence);
        uint64_t tell() const;
    private:
        std::vector<std::byte> m_memory;
        archive::inmem_input m_input;
    };
}

#endif //TOMBEXCAVATOR_IN_MEM_FILE_OPS_HH
