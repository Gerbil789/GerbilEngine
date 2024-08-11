#pragma once

#include <string>
#include <type_traits>
#include <memory>

namespace Engine 
{
    template <typename T>
    class has_create_method {
    private:
        template <typename U>
        static auto test(int) -> decltype(U::Create(std::string()), std::true_type());

        template <typename>
        static std::false_type test(...);

    public:
        static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
    };


    class Asset
    {
    public:
        virtual ~Asset() = default;

        template<typename T>
        static Ref<T> Create(const std::string& filePath)
		{
            ASSERT(has_create_method<T>::value, "T must have a static Create() method");
			return T::Create(filePath);
		}
    };
}