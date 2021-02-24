//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_FILE_SYSTEM_HH
#define TOMBEXCAVATOR_FILE_SYSTEM_HH

#include <string>
#include <memory>

#include <tombexcavator-provider_export.h>
#include <tomb-excavator/provider/vfs/directory.hh>
#include <tomb-excavator/provider/physfs/directory.hh>

namespace provider::vfs
{
    class PROVIDER_API file_system
    {
    public:
        file_system();
        virtual ~file_system();

        virtual std::string name() const = 0;
        virtual bool accept(const physfs::directory& dir) const = 0;
        virtual std::unique_ptr<directory> root(const physfs::directory& dir) const = 0;
    };
}

#if defined(PROVIDER_API_H)
#       define TE_PROVIDER_LOADER_NAME tombexcavator_provider_factory
#       undef PROVIDER_API_H
#       undef PROVIDER_API
#       if !defined(tombexcavator_provider_EXPORTS)
#           define tombexcavator_provider_EXPORTS
#       endif
#       include <tombexcavator-provider_export.h>
#           define REGISTER_PROVIDER_FS(TYPE) \
               extern "C" PROVIDER_API void* TE_PROVIDER_LOADER_NAME () { return new TYPE; }

#endif

#endif //TOMBEXCAVATOR_FILE_SYSTEM_HH
