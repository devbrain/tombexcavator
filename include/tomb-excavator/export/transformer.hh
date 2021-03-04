//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_TRANSFORMER_HH
#define TOMBEXCAVATOR_TRANSFORMER_HH

namespace exporter
{
    template <typename T>
    class transformer
    {
    public:
        virtual ~transformer() noexcept = default;

        void operator() (const T& input)
        {
            this->pre_transform(input);
            this->transform(input);
            this->post_transform(input);
        }
    protected:
        virtual void pre_transform([[maybe_unused]] const T& input)
        {
        }

        virtual void transform(const T& input) = 0;

        virtual void post_transform([[maybe_unused]] const T& input)
        {
        }
    };

}

#endif //TOMBEXCAVATOR_TRANSFORMER_HH
