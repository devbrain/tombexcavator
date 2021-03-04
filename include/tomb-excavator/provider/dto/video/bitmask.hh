//
// Created by igor on 24/02/2021.
//

#ifndef TOMBEXCAVATOR_BITMASK_HH
#define TOMBEXCAVATOR_BITMASK_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/dto/video/coord.hh>
#include <vector>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::dto
{
    class PROVIDER_API bitmask
    {
    public:
        bitmask(int w, int h);
        explicit bitmask(const dimension& dim);

        void set(int x, int y);
        [[nodiscard]] bool is_set(int x, int y) const;
        [[nodiscard]] const dimension& dim() const noexcept;
    private:
        dimension         m_dim;
        std::vector<bool> m_mask;
    };
} // ns
#include <tomb-excavator/msvc/c4251-end.h>

#endif //TOMBEXCAVATOR_BITMASK_HH
