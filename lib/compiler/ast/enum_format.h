#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <magic_enum.hpp>

#if !defined(MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT)
    #define MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT true
    #define MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT

namespace magic_enum::customize
{
// customize enum to enable/disable automatic std::format
template <typename E>
constexpr auto enum_format_enabled() noexcept -> bool
{
    return MAGIC_ENUM_DEFAULT_ENABLE_ENUM_FORMAT;
}
} // namespace magic_enum::customize

template <typename E>
struct fmt::formatter<
    E,
    std::enable_if_t<
        std::is_enum_v<E> && magic_enum::customize::enum_format_enabled<E>(),
        char>> : fmt::formatter<std::string_view, char>
{
    auto format(E e, format_context &ctx)
    {
        using D = std::decay_t<E>;
        if constexpr (magic_enum::detail::is_flags_v<D>)
        {
            if (auto name = magic_enum::enum_flags_name<D>(e); !name.empty())
            {
                return this->fmt::formatter<std::string_view, char>::format(
                    std::string_view{name.data(), name.size()}, ctx
                );
            }
        }
        else
        {
            if (auto name = magic_enum::enum_name<D>(e); !name.empty())
            {
                return this->fmt::formatter<std::string_view, char>::format(
                    std::string_view{name.data(), name.size()}, ctx
                );
            }
        }
        constexpr auto type_name = magic_enum::enum_type_name<E>();
        throw fmt::format_error(
            "Type of " + std::string{type_name.data(), type_name.size()} +
            " enum value: " + std::to_string(magic_enum::enum_integer<D>(e)) +
            " is not exists."
        );
    }
};
