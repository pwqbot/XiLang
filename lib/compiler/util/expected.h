#pragma once

#include <tl/expected.hpp>
#include <fmt/core.h>

namespace xi
{

template <typename T, typename E, typename F>
inline auto operator>>=(tl::expected<T, E> expected, F func)
{
    return expected.and_then(func);
}

} // namespace xi

template <typename T, typename E>
struct fmt::formatter<tl::expected<T, E>> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const tl::expected<T, E> & /*unused*/, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "expected");
    }
};
